#ifndef CLIENT_APP_H
#define CLIENT_APP_H

#include "NetworkManager.h"
#include <string>

class ClientApp {
public:
    ClientApp(NetworkManager& network);

    void run(); // 启动菜单循环

private:
    void doRegister();
    void doLogin();
    void doGetList();
    void doTalkToOne();
    void doTalkToGroup();
    void doExit();

    NetworkManager& _network;
    std::string _myName;
    bool _loggedIn;
};

#endif
