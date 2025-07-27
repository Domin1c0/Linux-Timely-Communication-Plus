#include "NetworkManager.h"
#include "ClientApp.h"
#include <iostream>

int main() {
    NetworkManager network;

    if (!network.connectToServer("127.0.0.1", 5555)) {
        std::cerr << "Failed to connect to server!\n";
        return 1;
    }

    network.startReceiving([](const std::string& msg) {
        std::cout << "\n\033[36m[Server] " << msg << "\033[0m\n";
    });

    ClientApp app(network);
    app.run();

    return 0;
}
