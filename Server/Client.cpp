#include <iostream>
#include "Customer.h"


int main(int argc, char const *argv[])
{
	TCPSocket* sock = new TCPSocket(HostAddres::LOOPBACK, 3334);
	sock->setErrorOutput(std::cout);
	Customer cus("Igor", sock);

	if(!sock->connectToServer())
		return -1;


	std::cout << "Client stared.\nEnter text: ";
	std::string str;
	
	
	while(1){
		std::cout << ">> ";
		std::getline(std::cin, str);

		str >> cus;
		str.clear();
		if(cus.read(str) == false){
			std::cout << "Out" << std::endl;
			break;
		}
		if(str.compare("close") == 0)
			break;
		std::cout << "Out text: " << str << std::endl;
	}	


	return 0;
}