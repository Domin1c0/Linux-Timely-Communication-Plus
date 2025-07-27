#include "NetworkManager.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

NetworkManager::NetworkManager() : _sockfd(-1), _running(false) {}

NetworkManager::~NetworkManager() {
    stop();
}

bool NetworkManager::connectToServer(const std::string& ip, int port) {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        perror("socket");
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);

    if (connect(_sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("connect");
        return false;
    }

    return true;
}

void NetworkManager::sendMessage(const std::string& msg) {
    send(_sockfd, msg.c_str(), msg.size(), 0);
}

void NetworkManager::startReceiving(MessageCallback callback) {
    _running = true;
    _recvThread = std::thread([this, callback]() {
        char buffer[1024];
        while (_running) {
            memset(buffer, 0, sizeof(buffer));
            int n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            if (n > 0) {
                callback(std::string(buffer));
            } else {
                break; // 断开连接
            }
        }
    });
}

void NetworkManager::stop() {
    _running = false;
    if (_sockfd != -1) {
        close(_sockfd);
        _sockfd = -1;
    }
    if (_recvThread.joinable()) {
        _recvThread.join();
    }
}
