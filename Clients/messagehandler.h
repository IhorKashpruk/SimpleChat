#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QObject>
#include "TCPSocket.h"
#include <set>
#include <QDebug>

enum class COMMAND{
    SEND =          0,
    TIME =          1,
    DATA =          2,
    CUSTOMERS =     3,
    AUTHORIZATION = 4,
    BAD_COMMAND =   5
};

struct Signal
{
   COMMAND command;
   std::set<std::string> words;
   std::string text;
};
Q_DECLARE_METATYPE(Signal)

const std::vector<std::string> customerCommandsList {
   "send", "time", "data", "customers", "authorization"
};


class MessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageHandler(TCPSocket* sock);
    void setSocket(TCPSocket* sock);
private:
    TCPSocket* socket;
    std::string lastMessage;
    struct timeval _timeVal;
    fd_set readSet;
    std::string buffer;
    bool isBufferEmpty();
    std::string cutMessage(std::string& str);

    void getCustomerSignal(const std::string& str2, Signal& signal);
    COMMAND getCustomerCommand(const std::string& str);
    void getWords(const std::string& str, std::set<std::string>& vec);
    bool waitSignal(const int timeval = 0);
signals:
    void sendSignal(Signal);
    void closeConnection();
public slots:
        void run();
};

#endif // MESSAGEHANDLER_H
