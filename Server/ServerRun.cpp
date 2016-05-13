#include "Server.h"
#include <iostream>


int main(int argc, char const *argv[])
{
	Server _server;

	if(!_server.listen(HostAddres::ANY, 3334, 10)){
		std::cout << "Error listen!\n";
		return -1;
	}
	_server.setErrorOutput(std::cout);
	std::cout << "Server started, time: " << __TIME__ << ", date: " << __DATE__ << std::endl;
	while(_server.waitSignal()){
		_server.accept();
		if(_server.executeSignal(_server.checkTerminal()) == -1){
			break;
		}
		_server.checkAllCustomers();
	}

	return 0;
}