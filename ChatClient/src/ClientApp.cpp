#include "ClientApp.h"
#include <jsoncpp/json/json.h>
#include <iostream>
#include <unistd.h>

ClientApp::ClientApp(NetworkManager& network)
    : _network(network), _loggedIn(false) {}

void ClientApp::run() {
    while (true) {
        if (!_loggedIn) {
            std::cout << "\033[32m1.Register  2.Login  3.Exit\033[0m\n";
            int choice;
            std::cin >> choice;
            switch (choice) {
                case 1: doRegister(); break;
                case 2: doLogin(); break;
                case 3: doExit(); return;
                default: std::cout << "Invalid choice!\n"; break;
            }
        } else {
            std::cout << "\033[31m1.Get List  2.Talk One  3.Talk Group  4.Exit\033[0m\n";
            int choice;
            std::cin >> choice;
            switch (choice) {
                case 1: doGetList(); break;
                case 2: doTalkToOne(); break;
                case 3: doTalkToGroup(); break;
                case 4: doExit(); return;
                default: std::cout << "Invalid choice!\n"; break;
            }
        }
    }
}

void ClientApp::doRegister() {
    std::string name, pw;
    std::cout << "Enter name: "; std::cin >> name;
    std::cout << "Enter password: "; std::cin >> pw;

    Json::Value val;
    val["reason_type"] = 1; // MSG_TYPE_REGISTER
    val["name"] = name;
    val["pw"] = pw;

    _network.sendMessage(val.toStyledString());
}

void ClientApp::doLogin() {
    std::string name, pw;
    std::cout << "Enter name: "; std::cin >> name;
    std::cout << "Enter password: "; std::cin >> pw;

    Json::Value val;
    val["reason_type"] = 2; // MSG_TYPE_LOGIN
    val["name"] = name;
    val["pw"] = pw;

    _network.sendMessage(val.toStyledString());
    _myName = name;
    _loggedIn = true;
}

void ClientApp::doGetList() {
    Json::Value val;
    val["reason_type"] = 5; // MSG_TYPE_GET_LIST
    val["name"] = _myName;
    _network.sendMessage(val.toStyledString());
}

void ClientApp::doTalkToOne() {
    std::string herName;
    std::cout << "Enter friend name: ";
    std::cin >> herName;

    std::cin.ignore();
    std::string message;
    std::cout << "Message: ";
    std::getline(std::cin, message);

    Json::Value val;
    val["reason_type"] = 3; // MSG_TYPE_TALK_TO_ONE
    val["name"] = _myName;
    val["hername"] = herName;
    val["message"] = message;

    _network.sendMessage(val.toStyledString());
}

void ClientApp::doTalkToGroup() {
    std::string friends;
    std::cin.ignore();
    std::cout << "Enter friends (separated by ';'): ";
    std::getline(std::cin, friends);

    std::string message;
    std::cout << "Message: ";
    std::getline(std::cin, message);

    Json::Value val;
    val["reason_type"] = 4; // MSG_TYPE_TALK_TO_GROUP
    val["name"] = _myName;
    val["hername"] = friends;
    val["message"] = message;

    _network.sendMessage(val.toStyledString());
}

void ClientApp::doExit() {
    Json::Value val;
    val["reason_type"] = 6; // MSG_TYPE_EXIT
    val["name"] = _myName;
    _network.sendMessage(val.toStyledString());
    _network.stop();
    std::cout << "Exiting...\n";
}
