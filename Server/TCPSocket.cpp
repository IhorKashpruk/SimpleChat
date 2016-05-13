#include "TCPSocket.h"
// Constructors and destructor
TCPSocket::TCPSocket(const HostAddres hAddress, const int port)
	: isConnected(false)
{
	uint32_t hostAddress;
	switch(hAddress){
		case HostAddres::LOOPBACK: hostAddress = INADDR_LOOPBACK; break;
		case HostAddres::ANY: hostAddress = INADDR_ANY; break;
		default: break;
	}
	_address.sin_family = AF_INET;
  	_address.sin_port = htons(port);
  	_address.sin_addr.s_addr = htonl(hostAddress); 

  	if((_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1){
  		err.showErrorLocal(std::strerror(errno));
  	}
}

TCPSocket::TCPSocket(const int descriptor, sockaddr_in& address, const bool connected = false){
	_address = address;
	_descriptor = descriptor;
	isConnected = connected;
}

TCPSocket::TCPSocket(){
	isConnected = false;
}

TCPSocket::~TCPSocket()
{
	close();
}

// Methods

bool TCPSocket::create(const HostAddres hAddress, const int port){
	uint32_t hostAddress;
	switch(hAddress){
		case HostAddres::LOOPBACK: hostAddress = INADDR_LOOPBACK; break;
		case HostAddres::ANY: hostAddress = INADDR_ANY; break;
		default: break;
	}
	_address.sin_family = AF_INET;
  	_address.sin_port = htons(port);
  	_address.sin_addr.s_addr = htonl(hostAddress); 

  	if((_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
  		err.showErrorLocal(std::strerror(errno));
  		return false;
  	}
  	return true;
}

bool TCPSocket::connectToServer(){
	if(connect(_descriptor, (struct sockaddr*)&_address, sizeof(_address)) == -1){
		err.showErrorLocal(std::strerror(errno));
		return false;
	}
	isConnected = true;
	return true;
}
bool TCPSocket::bind(){
	int yes = 1;
	if(setsockopt(_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    	err.showErrorLocal(std::strerror(errno));
    	return false;
	}
	if(::bind(_descriptor, (struct sockaddr*)&_address, sizeof(_address)) < 0){
		err.showErrorLocal(std::strerror(errno));
		return false;
	}
	return true;
}
TCPSocket* TCPSocket::accept(){
	int new_client;
	struct sockaddr_in _addr_new_sock;
	socklen_t len = sizeof(_addr_new_sock);
	if((new_client = ::accept(_descriptor, (struct sockaddr*)&_addr_new_sock, &len)) == -1){
		err.showErrorLocal(std::strerror(errno));
		return NULL;
	} 
	return new TCPSocket(new_client, _addr_new_sock);
}
int TCPSocket::readAll(std::string& str){
	str.clear();
	int size = 1024;
	char buff[size];
	int n = -1;
	int countEAGAIN = 0;
	while(1){
		memset(buff, 0, size);
		if((n = recv(_descriptor, buff, size - 1, MSG_DONTWAIT)) == -1){
			if(errno == EAGAIN){
				countEAGAIN++;
			if(countEAGAIN < 2)
				continue;
			else 
				break;
			}
		

			err.showErrorLocal(std::strerror(errno));
			if(str.length() == 0)
				return -1;
			break;
		}
		if(n == 0){
			break;
		}
		str += std::string(buff);
	}
	return str.length();
}
int TCPSocket::read(std::string& str, const int size = 1024){
	char buff[size];
	memset(buff, 0, size);
	int n = -1;
	if((n = recv(_descriptor, buff, size, 0)) == -1){
		err.showErrorLocal(std::strerror(errno));
		return -1;
	}
	str = std::string(buff);
	return n;
}
int TCPSocket::write(const std::string& str){
	int n = -1;
	if((n = send(_descriptor, str.c_str(), str.length(), 0)) == -1){
		err.showErrorLocal(std::strerror(errno));
		return -1;
	}
	return n;
}
bool TCPSocket::close(){
	if(::close(_descriptor) == -1){
		err.showErrorLocal(std::strerror(errno));
		return false;
	}
	return true;
}

bool TCPSocket::listen(const int count){
	if(::listen(_descriptor, count) == -1){
		err.showErrorLocal(std::strerror(errno));
		return false;
	}
	return true;
}

int TCPSocket::getDescriptor(){
	return _descriptor;
}

void TCPSocket::setErrorOutput(std::ostream& os){
	err.setPointOutLocal(os, "Socket(" + std::to_string(_descriptor) + ", " + getIp() + "):\n");
}

std::string TCPSocket::getIp(){
	char str[INET_ADDRSTRLEN];
	if(inet_ntop(AF_INET, &(_address.sin_addr), str, INET_ADDRSTRLEN) == NULL){
		err.showErrorLocal(std::strerror(errno));
		return std::string("0.0.0.0");
	}
	return std::string(str);
}

// Operators

TCPSocket::operator int(){
	return _descriptor;
}