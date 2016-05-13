#include "client.h"
#include <exception>

// Constructors and destructors
Client::Client(const HostAddres hAddress, const int port)
    : connected(false), authorized(false), autoMode(false), hostAdress(hAddress), socketPort(port)
{
    account = {"", ""};
    socket_.setErrorOutput(std::cout);
    socket_.create(hAddress, port);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeoutConnectionToServerAutoModeSlot()));
}

Client::Client() : connected(false), authorized(false), autoMode(false)
{
    hostAdress = HostAddres::LOOPBACK;
    account = {"", ""};
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeoutConnectionToServerAutoModeSlot()));
}

Client::~Client(){
    if(timer.isActive())
        timer.stop();
}


// Methods
Account Client::getAccount(){
    return account;
}

bool Client::isConnection() const {
    return connected;
}

bool Client::isAuthorization() const {
    return authorized;
}


void Client::setHostAdressAndPort(const HostAddres hAddress, const int port){
    hostAdress = hAddress;
    socketPort = port;
    if(connected)
        closeConnection();
    socket_.create(hAddress, port);
}

void Client::setLoginAndPassword(std::string login, std::string password){
    account.login = login;
    account.password = password;
}

void Client::closeConnection(){
    socket_.close();
    connected = false;
    authorized = false;
    autoMode = false;
    socket_.create(hostAdress, socketPort);
    emit CloseConnectionSignal();
    emit InformationSignal("Connection closed");
}

bool Client::connectToServer(){
    if(connected){
        closeConnection();
    }
    if(socket_.connectToServer()){
        connected = true;
        createMessageHandler();
        emit ConnectionToServerSignal(true);
        emit InformationSignal("Connection success");
        return true;
    }
    emit ConnectionToServerSignal(false);
    emit InformationSignal("Connection not success");
    return false;
}

bool Client::connectToServer(HostAddres hAddress, const int port){
    hostAdress = hAddress;
    socketPort = port;
    return connectToServer();
}

void Client::authorization(){
    if(account.login.empty() || account.password.empty()){
        emit InformationSignal("Set login and password");
        return;
    }
    if(!connected){
        emit InformationSignal("Client is not connected");
        return;
    }
    sendMessageToServer("authorizate " + account.login + " " + account.password);
}

void Client::authorization(std::string login, std::string password){
    account.login = login;
    account.password = password;
    authorization();
}

bool Client::sendMessageToServer(std::string str){
    if(!connected){
        emit InformationSignal("Client is not connected");
        return false;
    }

    return socket_.write(str);
}

bool Client::sendMessageToServer(const Signal &signal){
    if(!connected){
        emit InformationSignal("Client is not connected");
        return false;
    }
    std::string str;
    switch(signal.command){
        case COMMAND::AUTHORIZATION: str = "authorizate "; break;
        case COMMAND::CUSTOMERS: str = "customers "; break;
        case COMMAND::DATA: str = "data "; break;
        case COMMAND::SEND: str = "send "; break;
        case COMMAND::TIME: str = "time "; break;
        case COMMAND::BAD_COMMAND: return false;
    }

    for(auto& it: signal.words){
        str += it + " ";
    }

    if(!signal.text.empty()){
        str += "'" + signal.text + "'";
    }
    emit InformationSignal(QString(str.c_str()));
    return sendMessageToServer(str);
}


void Client::createMessageHandler(){
    MessageHandler* MH = new MessageHandler(&socket_);
    QThread* th = new QThread();
    MH->moveToThread(th);

    connect(th, SIGNAL(started()), MH, SLOT(run()));
    connect(th, SIGNAL(finished()), th, SLOT(deleteLater()));
    connect(th, SIGNAL(finished()), MH, SLOT(deleteLater()));
    connect(MH, SIGNAL(closeConnection()), th, SLOT(quit()));
    qRegisterMetaType<Signal>();
    connect(MH, SIGNAL(sendSignal(Signal)), this, SLOT(getSignal(Signal)));
    connect(MH, SIGNAL(closeConnection()), this, SLOT(closeConnectonFromMHSlot()));

    th->start();
}

// AutoMode
void Client::connectToServerAutoMode(const uint interval){
    if(autoMode){
        return;
    }
    autoMode = true;
   if(connected){
       emit InformationSignal("Client connected");
       autoMode = false;
       return;
   }

   timer.start(interval);

}

void Client::connectAndAuthorizationAutoMode(){
    connectToServerAutoMode();
    autoMode = true;
    connect(this, SIGNAL(ConnectionToServerSignal(bool)), this, SLOT(checkConnectAndAuthorizationAutoModeSlot(bool)));
}

void Client::stopAutoMode(){
    if(timer.isActive()){
        timer.stop();
        autoMode = false;
    }
}

// SLOTS
void Client::timeoutConnectionToServerAutoModeSlot(){
    if(connectToServer()){
        timer.stop();
        autoMode = false;
    }
}

void Client::checkConnectAndAuthorizationAutoModeSlot(bool result){
    if(!result)
        return;
    authorization();
    disconnect(this, SIGNAL(ConnectionToServerSignal(bool)), this, SLOT(checkConnectAndAuthorizationAutoModeSlot(bool)));
    autoMode = false;
}

void Client::closeConnectonFromMHSlot(){
    closeConnection();
}

void Client::getSignal(Signal signal){
//    if(signal.text.empty()){
//        return;
//    }
    switch(signal.command){
    case COMMAND::SEND:
        if(signal.words.size() != 0)
            for(auto& it: signal.words)
                emit FromServerSendSignal(it, signal.text);
        break;
    case COMMAND::DATA:
        emit FromServerDataSignal(signal.text); break;
    case COMMAND::TIME:
        emit FromServerTimeSignal(signal.text); break;
    case COMMAND::CUSTOMERS:
        if(signal.words.size() != 0)
            emit FromServerCustomersSignal(signal.words); break;
    case COMMAND::AUTHORIZATION:
        if(signal.text.compare("good") == 0){
            emit InformationSignal("Authorization good");
            emit AuthorizationSignal(true);
        }
        else{
            emit InformationSignal("Authorization bad");
            emit AuthorizationSignal(false);
        }
        break;
    case COMMAND::BAD_COMMAND:
        break;
    }
}
