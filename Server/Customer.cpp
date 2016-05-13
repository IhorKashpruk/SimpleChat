#include "Customer.h"

// Constructors, destructors and operators
Customer::Customer(const std::string login, TCPSocket* socket)
	: _login(login), _socket(socket)
{
}

Customer::~Customer(){
	if(_socket != NULL){
		delete _socket;
	}
}

Customer::operator std::string(){
	return _login;
}

Customer::operator int(){
	return _socket->getDescriptor();
}

bool operator >>(Customer& obj, std::string& str){
	if(obj._socket->readAll(str) < 1)
		return false;
	return true;
}

bool operator >>(std::string& str, Customer& obj){
	if(obj._socket->write(str) == -1)
		return false;
	return true;
} 

// Methods

std::string Customer::login(){
	return _login;
}
std::string Customer::Info(){
	return std::string("Login: \"" + _login + "\", ip: " + _socket->getIp());
}

void Customer::deleteSocket(){
	if(_socket != NULL){
		_socket->close();
		delete _socket;
	}
}

bool Customer::read(std::string& str, const int len){
	if(_socket->read(str, len)  < 1)
		return false;
	return true;
}


void Customer::changeLogin(const std::string str){
	_login = str;
}