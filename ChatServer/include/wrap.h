#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring> // For strerror()

int Socket(int domain, int type, int protocol) {
    int sockfd;
    if ((sockfd = socket(domain, type, protocol)) < 0) {
        throw std::runtime_error("Socket creation failed: " + std::string(strerror(errno)));
    }
    return sockfd;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) < 0) {
        throw std::runtime_error("Bind failed: " + std::string(strerror(errno)));
    }
}

void Listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        throw std::runtime_error("Listen failed: " + std::string(strerror(errno)));
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int connfd;
    if ((connfd = accept(sockfd, addr, addrlen)) < 0) {
        throw std::runtime_error("Accept failed: " + std::string(strerror(errno)));
    }
    return connfd;
}

void Close(int fd) {
    if (close(fd) < 0) {
        throw std::runtime_error("Close failed: " + std::string(strerror(errno)));
    }
}
