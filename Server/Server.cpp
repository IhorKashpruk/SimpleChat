#include "Server.h"


// Constructors and destructors
Server::Server(){
	_coutWaitConnecting = 0;

	FD_ZERO(&readSet);
}

Server::~Server(){
	closeCustomer(_notAuthorCustomers);
    closeCustomer(_customers);
}

// Methods

bool Server::listen(const HostAddres hAddress, const int port, const int n){
	if(!_listener.create(hAddress, port))
		return false;
	if(!_listener.bind()){
		return false;
	}
	return _listener.listen(n);
}

bool Server::waitSignal(const int timeval){
    _timeVal.tv_sec = timeval;
    _timeVal.tv_usec = 0;

    FD_ZERO(&readSet);

    FD_SET(fileno(stdin), &readSet);
    FD_SET(_listener, &readSet);
    int maxDescriptor = fileno(stdin) < _listener ? _listener : fileno(stdin);

    for(auto& it: _customers){
    	FD_SET(*it.second, &readSet);
    	if(maxDescriptor < *it.second)
    		maxDescriptor = *it.second;
    }
    
    for(auto& it: _notAuthorCustomers){
    	FD_SET(*it.second, &readSet);
    	if(maxDescriptor < *it.second)
    		maxDescriptor = *it.second;
    }
    
    int result;
    if(timeval == 0){
    	result = select(maxDescriptor+1, &readSet, NULL, NULL, NULL);
    } 
    else
    	result = select(maxDescriptor+1, &readSet, NULL, NULL, &_timeVal);

    switch(result){
    	case -1: 
    	    err.showErrorLocal(std::strerror(errno));
    		return false;
    		break;
    	case 0:
    		err.showErrorLocal("Time out.");
            return false;
    		break;
    }

    return true;

}

// Перевіряє команди написані в терміналi
Signal<SERVER_COMMAND> Server::checkTerminal(){
    if(FD_ISSET(fileno(stdin), &readSet)){
        char send_line[1025];
        
        memset(send_line, 0, 1024);
        if(fgets(send_line, 1024, stdin) == NULL){
            return Signal<SERVER_COMMAND> {SERVER_COMMAND::NO_COMMAND};
        }
        std::string str(send_line, strlen(send_line)-1);

        // Визначаемо сигнал

        Signal<SERVER_COMMAND> signal(SERVER_COMMAND::BAD_COMMAND);

        SignalManager::getServerSignal(str, signal);

        return signal;
        
    }
    return Signal<SERVER_COMMAND> {SERVER_COMMAND::NO_COMMAND};
}

Signal<CUSTOMER_COMMAND> Server::checkClient(Customer& cust){
    std::string str;
    std::string message;
    if(FD_ISSET(cust, &readSet)){
        if(cust >> str){
            Signal<CUSTOMER_COMMAND> signal(CUSTOMER_COMMAND::BAD_COMMAND);
            SignalManager::getCustomerSignal(str, signal);
            return signal;
        }
        else{
            err.showInfoLocal("Delete client: ", cust);
            closeCustomer(cust, _customers);
            for(auto& it: _customers)
                message += it.first + " "; 
            sendMessage("/*customers " + message + "*/", _customers);
        }
    }

    return Signal<CUSTOMER_COMMAND> {CUSTOMER_COMMAND::NO_COMMAND};
}

int Server::executeSignal(Signal<SERVER_COMMAND> signal){

    // pointer to map _cutomer or _notAuthorCustomer
    auto* map = signal.words.find("NAC") != signal.words.end() ? &_notAuthorCustomers : &_customers; 
    std::vector<std::string> clientList;
    std::string text = "send Server " + std::string("'");

    switch(signal.command){
        case SERVER_COMMAND::SEND:
            if(signal.text.compare("") == 0){
                err.showMessageWithTitle("send", "error synatx - no text.");
                return 0;
            }
            if(signal.words.size() == 0){
                err.showMessageWithTitle("send", "error syntax - no customers.");
                return 0;
            }
            if(signal.words.find("everybody") != signal.words.end()){
                text += signal.text + "'";
                sendMessage(text, *map);
                signal.words.erase("everybody");
            }
            
            for(auto& it: signal.words){
                text += signal.text + "'";
                if(!sendMessage("/*" + text + "*/", it, *map))
                    err.showMessageWithTitle("send", it + " not find", false);
            }
            err.addNewLnOs2();
            break;
        case SERVER_COMMAND::CLOSE:
            if(signal.words.size() == 0){
                err.showInfoLocal("Close", "error syntax: no customer");
                return 0;
            }
            if(signal.words.find("NAC") != signal.words.end())
                signal.words.erase("NAC");
            if(signal.words.find("AC") != signal.words.end())
                signal.words.erase("AC");

            if(signal.words.find("everybody") != signal.words.end()){
                closeCustomer(*map);
                err.showMessageWithTitle("Close", "everybody closed.");
                break;
            }else
            for(auto& it: signal.words){
                if(!closeCustomer(it, *map))
                    err.showMessageWithTitle("Close", it + " not find", false);
            }
            err.addNewLnOs2();
            break;
        case SERVER_COMMAND::EXIT:
            err.showMessageWithTitle("EXIT", "bye\n", false);
            return -1;
            
        case SERVER_COMMAND::HELP:
            err.showInfoLocal("Help information", "\n1) send name or everybody 'text' - send text to name or everybody.\n2) close name or everybody || close NAC or AC name or everybody.\n- close name or everybody;\n- NAC - not authorized customers, AC - authorized customers.\n3) help - help information.\n4) clients NAC or AC - show clients. Default set AC.");
            break;
        case SERVER_COMMAND::CLIENTS:
            for(auto& it: *map)
                clientList.push_back(it.first);
            err.showListMessage("Clinet list", clientList);
            break; 
        case SERVER_COMMAND::BAD_COMMAND:
            err.showMessageWithTitle("Command", "bad command");
            break;
        case SERVER_COMMAND::NO_COMMAND:
            break; 
    }

    return 1;
}
bool Server::executeSignal(Signal<CUSTOMER_COMMAND> signal, Customer* custom){
    std::vector<std::string> v;
    std::string message;

        // pointer to map _cutomer or _notAuthorCustomer
    auto* map = signal.words.find("NAC") != signal.words.end() ? &_notAuthorCustomers : &_customers; 

    switch(signal.command){
        case CUSTOMER_COMMAND::SEND:
            if(signal.text.compare("") == 0){
                sendMessage("/*send 'bad syntax no text'*/", custom);
                return true;
            }
            if(signal.words.size() == 0){
                sendMessage("/*send 'bad syntax no customer'*/", custom);
                return true;
            }
            if(signal.words.find("everybody") != signal.words.end()){
                sendMessage("/*send " + custom->login() + std::string(" '") + signal.text + "'*/", *map);
                signal.words.erase("everybody");
            }
            
            for(auto& it: signal.words){
                sendMessage("/*send " + custom->login() + std::string(" '") + signal.text + "'*/", it, *map);
            }
            return true;
        case CUSTOMER_COMMAND::TIME:
            _currentTime = time(0);
            _timeInfo = localtime(&_currentTime);
            sendMessage("/*time '" + std::to_string(_timeInfo->tm_hour) + ":"
                + std::to_string(_timeInfo->tm_min) + ":"
                + std::to_string(_timeInfo->tm_sec) + "'*/", custom);
            return true;
        case CUSTOMER_COMMAND::DATA:
            message = "data";
            _currentTime = time(0);
            _timeInfo = localtime(&_currentTime);
            sendMessage("/*data '" + std::to_string(1900 + _timeInfo->tm_year) + " "
                + std::to_string(1+_timeInfo->tm_mon) + " "
                + std::to_string(_timeInfo->tm_mday) + "'*/", custom);
            return true;
        case CUSTOMER_COMMAND::CUSTOMERS:
            for(auto& it: *map)
                message += it.first + " "; 
            sendMessage("/*customers " + message + "*/", custom);
            return true;
        case CUSTOMER_COMMAND::BAD_COMMAND:
            message = "bad command";
            sendMessage("/*badcommand '" + message + "'*/", custom);
            break;
        case CUSTOMER_COMMAND::NO_COMMAND:
            return true;
    }
    return false;
}

bool Server::accept(){
	if(FD_ISSET(_listener, &readSet)){
		std::string login = std::to_string(_notAuthorCustomers.size()+1);
		TCPSocket* new_socket = _listener.accept();
		new_socket->setErrorOutput(*err.getOS());
		_notAuthorCustomers.insert(std::pair<std::string, Customer*>
			(login, new Customer(login, new_socket))
		);
        err.showInfoLocal("accept", _notAuthorCustomers.find(login)->second->Info());
        return true;
	}
    return false;
}
void Server::checkAllCustomers(){
    static Signal<CUSTOMER_COMMAND> sign(CUSTOMER_COMMAND::NO_COMMAND);

    size_t size_set = _customers.size();
    // First checking authorized users
    int x = 0;
    for(auto it = _customers.begin(); it != _customers.end(); ++it, x++){
        sign = checkClient(*it->second);
        if(_customers.size() == 0)
        	break;
        if(size_set != _customers.size()){
        	size_set--;
        	it = _customers.begin();
        	for(int i = 0; i < x-1; i++){
        		it++;
        	}
        	continue;
        }
        if(sign.command != CUSTOMER_COMMAND::NO_COMMAND)
          executeSignal(sign, it->second);
    }
    // checking no authorized users


    std::vector<std::string> vectorNACToErase;

    std::string message_good = "/*authorization 'good'*/";
    std::string message_bad = "/*authorization 'bad'*/";
    std::string message;

	for(auto& it: _notAuthorCustomers){
        std::string str;
        if(FD_ISSET(*it.second, &readSet)){
            if(*it.second >> str){
                if(authorizate(str, it.second)){
                    sendMessage(message_good , it.second);
                    vectorNACToErase.push_back(it.first);
                    for(auto& it: _customers)
                		message += it.first + " "; 
            		sendMessage("/*customers " + message + "*/", _customers);
                }
                else{
                    sendMessage(message_bad, it.second);
                }
            }
            else{
                err.showMessageWithTitle("Delete client: ", it.first);
                closeCustomer(*it.second, _notAuthorCustomers);
            }
        }
    }
    for(auto& it: vectorNACToErase){
       _notAuthorCustomers.erase(it);
   }
}
void Server::setErrorOutput(std::ostream& os){
	err.setPointOutLocal(os, "Server(" + std::to_string(_listener.getDescriptor()) + ", " + _listener.getIp() + "):");
}

bool Server::sendMessage(std::string&& str, Customer* customer){
    std::string str2 = str;
    return str2 >> *customer;
}

bool Server::sendMessage(std::string& str, Customer* customer){
    return str >> *customer;
}

bool Server::sendMessage(std::string& str, std::map<std::string, Customer*>& customers){
    bool result = true;
    for(auto& it: customers){
            if(!(str >> *it.second))
                result = false;
    }
    return result;
}

bool Server::sendMessage(std::string&& str, std::map<std::string, Customer*>& customers){
	std::string str2 = str;
	return sendMessage(str2, customers);
}

bool Server::sendMessage(std::string&& str, const std::string name, std::map<std::string, Customer*>& customers){
	std::string str2 = str;
	return sendMessage(str2, name, customers);
}

bool Server::sendMessage(std::string& str, const std::string name, std::map<std::string, Customer*>& customers){
    std::map<std::string, Customer*>::iterator it = customers.find(name);
    if(it == customers.end()){
        return false;
    }

    return str >> *(it->second);
}
bool Server::closeCustomer(const std::string name, std::map<std::string, Customer*>& customers){
    std::map<std::string, Customer*>::iterator it = customers.find(name);
    if(it == customers.end())
        return false;
    delete it->second;
    customers.erase(it->first);
    return true;
}
void Server::closeCustomer(std::map<std::string, Customer*>& customers){
    for(auto& it: customers)
        delete it.second;

    customers.clear();
}


bool Server::authorizate(std::string& str, Customer* client){
    std::vector<std::string> v;
    SignalManager::removeExtraSpaces(str);
    SignalManager::saveWordsInVector(str, v);

    if(v.size() != 3){
        return false;
    }

    if(v.at(0).compare("authorizate") != 0){
        return false;
    }

    // Перевірити логін і пароль
    if(_customers.find(v.at(1)) != _customers.end())
    	return false;

    if(v.size() != 3)
        return false;

    // Переводимо з _notAuthorCustomers в _customer
    client->changeLogin(v.at(1));
    _customers.insert(std::pair<std::string, Customer*>(v.at(1), &*client));
    err.showInfoLocal("authorizate",  "Customer " + v.at(1) + " autorizated.");
    return true;
}
