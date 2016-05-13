#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTimer>
#include "TCPSocket.h"
#include "messagehandler.h"
#include <iostream>
#include <QThread>

typedef unsigned int uint;

struct Account{
    std::string login;
    std::string password;
};

class Client : public QObject
{
    Q_OBJECT
public:
    Client(const HostAddres hAddress, const int port);
    Client();
    ~Client();

    void setHostAdressAndPort(const HostAddres hAddress, const int port);
    void setLoginAndPassword(std::string login, std::string password);

    bool connectToServer();
    bool connectToServer(HostAddres hAddress, const int port);
    void authorization();
    void authorization(std::string login, std::string password);
    void closeConnection();

    void connectToServerAutoMode(const uint interval = 3000);
    void connectAndAuthorizationAutoMode();
    void stopAutoMode();

    bool isConnection() const;
    bool isAuthorization() const;

    bool sendMessageToServer(std::string str);
    bool sendMessageToServer(const Signal& signal);
    Account getAccount();

private:
    TCPSocket socket_;
    bool connected;
    bool authorized;
    bool autoMode;
    QTimer timer;
    HostAddres hostAdress;
    Account account;
    int socketPort;

    void createMessageHandler();
signals:
    void ConnectionToServerSignal(bool);
    void CloseConnectionSignal();
    void AuthorizationSignal(bool);

    void ErrorSignal(QString message);
    void InformationSignal(QString message);

    void FromServerSendSignal(std::string customer, std::string text);
    void FromServerDataSignal(std::string text);
    void FromServerTimeSignal(std::string text);
    void FromServerCustomersSignal(std::set<std::string>& customers);
private slots:
    void timeoutConnectionToServerAutoModeSlot();
    void checkConnectAndAuthorizationAutoModeSlot(bool result);
    void getSignal(Signal);
    void closeConnectonFromMHSlot();
};

#endif // CLIENT_H
