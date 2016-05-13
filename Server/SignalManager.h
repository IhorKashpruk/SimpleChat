#ifndef _SIGNALMANAGER_H_
#define _SIGNALMANAGER_H_
#include <set>
#include <string>
#include <vector>

enum class SERVER_COMMAND
{
   SEND = 0, 
   CLOSE = 1,
   EXIT = 2,
   HELP = 3,
   CLIENTS = 4,
   BAD_COMMAND = 5,
   NO_COMMAND = 6
};

const std::vector<std::string> serverCommandsList {
   "send", "close", "exit", "help", "clients"
};

enum class CUSTOMER_COMMAND
{
   SEND = 0, 
   TIME = 1,
   DATA = 2,
   CUSTOMERS = 3,
   BAD_COMMAND = 4,
   NO_COMMAND = 5
};

const std::vector<std::string> customerCommandsList {
   "send", "time", "data", "customers"
};

template <class T>
struct Signal
{
   T command;
   std::set<std::string> words;
   std::string text; 
   explicit Signal(T com){
      command = com;
   }
};

class SignalManager
{
private:
	SignalManager(){}

	static void getWords(const std::string& str, std::set<std::string>& vec);
	static SERVER_COMMAND getServerCommand(const std::string& str);
	static CUSTOMER_COMMAND getCustomerCommand(const std::string& str);
public:
	static void getServerSignal(const std::string& str2, Signal<SERVER_COMMAND>& signal);
	static void getCustomerSignal(const std::string& str2, Signal<CUSTOMER_COMMAND>& signal);
   static std::string getWordsToNumber(const std::string& str, const int n = 1);
   static void saveWordsInVector(const std::string& str, std::vector<std::string>& out_vec);
   static void removeExtraSpaces(std::string& str, size_t start = 0, size_t end = 0);
};


#endif // _SIGNALMANAGER_H_