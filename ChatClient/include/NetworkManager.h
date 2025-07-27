#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>

class NetworkManager {
public:
    using MessageCallback = std::function<void(const std::string&)>;

    NetworkManager();
    ~NetworkManager();

    bool connectToServer(const std::string& ip, int port);
    void sendMessage(const std::string& msg);
    void startReceiving(MessageCallback callback);
    void stop();

private:
    int _sockfd;
    std::thread _recvThread;
    std::atomic<bool> _running;
};

#endif
