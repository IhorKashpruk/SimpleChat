#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include "Oerror.h"

enum class HostAddres{
	LOOPBACK,
	ANY
};

class TCPSocket
{
private:
	struct sockaddr_in _address;
	int _descriptor;
	bool isConnected;
	Oerror err;
public:
	TCPSocket(const HostAddres hAddress, const int port);
	TCPSocket(const int descriptor, sockaddr_in& address, const bool connected);
	TCPSocket();
	~TCPSocket();
	bool create(const HostAddres hAddress, const int port);
	bool connectToServer();
	bool bind();
	TCPSocket* accept();
	bool listen(const int count);
	int readAll(std::string& str);
	int read(std::string& str, const int size);
	int write(const std::string& str);
	bool close();
	int getDescriptor();
	void setErrorOutput(std::ostream& os);
	std::string getIp();
	operator int();
};

#endif //_TCPSOCKET_H_
