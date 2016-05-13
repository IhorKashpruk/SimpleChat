#ifndef _MSERVER_H_
#define _MSERVER_H_
#include "Customer.h"
#include "SignalManager.h"
#include <map>
#include <algorithm>
#include <vector>
#include "Oerror.h"
#include <ctime>

class Server
{
private:
	TCPSocket _listener;
	std::map<std::string, Customer*> _customers;
	std::map<std::string, Customer*> _notAuthorCustomers;
	time_t _currentTime;
	struct tm* _timeInfo;

	int _coutWaitConnecting;
	Oerror err;

	struct timeval _timeVal;
	fd_set readSet;

	bool authorizate(std::string& str, Customer* client);

	bool sendMessage(std::string&&, Customer*);
	bool sendMessage(std::string&, Customer*);
	bool sendMessage(std::string&, std::map<std::string, Customer*>&);
	bool sendMessage(std::string&&, std::map<std::string, Customer*>&);
	bool sendMessage(std::string&, const std::string, std::map<std::string, Customer*>&);
	bool sendMessage(std::string&&, const std::string, std::map<std::string, Customer*>&);

	bool closeCustomer(const std::string, std::map<std::string, Customer*>& customers);
	void closeCustomer(std::map<std::string, Customer*>&);

public:
	Server();
	~Server();
	bool listen(const HostAddres hAddress, const int port, const int n);
	bool waitSignal(const int timeval = 0);
	bool accept();
	void checkAllCustomers();
	Signal<SERVER_COMMAND> checkTerminal();
	Signal<CUSTOMER_COMMAND> checkClient(Customer&);

	int executeSignal(Signal<SERVER_COMMAND> signal); // When returned -1: exit with program
	bool executeSignal(Signal<CUSTOMER_COMMAND> signal, Customer* custom); 

	void setErrorOutput(std::ostream& os);


};

#endif // _MSERVER_H_