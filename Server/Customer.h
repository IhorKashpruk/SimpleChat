#ifndef _CUSTOMER_H_
#define _CUSTOMER_H_

#include "TCPSocket.h"

class Customer
{
private:
	TCPSocket* const _socket;
	std::string _login;

public:
	Customer(const std::string login, TCPSocket* socket);
	~Customer();
	Customer(const Customer&) = delete;
	Customer& operator=(Customer&) = delete;
	operator std::string();
	operator int();
	friend bool operator >>(Customer& obj, std::string& str);
	friend bool operator >>(std::string& str, Customer& obj);

	std::string Info();
	void deleteSocket();
	bool read(std::string& str, const int len = 1024);
	void changeLogin(const std::string str);
	std::string login();
};

#endif // _CUSTOMER_H_