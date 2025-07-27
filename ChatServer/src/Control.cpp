#include "Control.h"
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>

Control::Control(MysqlWrapper* db, std::map<int, struct event*>& eventMap)
{
    _map[MSG_TYPE_REGISTER]    = new Register(db);
    _map[MSG_TYPE_LOGIN]       = new Login(db);
    _map[MSG_TYPE_TALK_TO_ONE] = new Talk_one(db);
    _map[MSG_TYPE_TALK_TO_GROUP] = new Talk_group(db);
    _map[MSG_TYPE_GET_LIST]    = new Get_List(db);
    _map[MSG_TYPE_EXIT]        = new Exit(db, eventMap);
}

Control::~Control()
{
    for (auto& pair : _map)
    {
        delete pair.second;
    }
    _map.clear();
}

void Control::process(int fd, std::string& json)
{
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(json, root))
    {
        std::cerr << "[Control] JSON parse failed: " << json << std::endl;
        std::string error = "Invalid JSON format";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }

    if (!root.isMember("reason_type"))
    {
        std::cerr << "[Control] Missing reason_type" << std::endl;
        std::string error = "Missing reason_type";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }

    int reasonType = root["reason_type"].asInt();

    auto it = _map.find(reasonType);
    if (it == _map.end())
    {
        std::cerr << "[Control] Unknown reason_type: " << reasonType << std::endl;
        std::string error = "Unknown request type";
        send(fd, error.c_str(), error.size(), 0);
        return;
    }

    try {
        it->second->process(fd, json);
        it->second->response();
    } 
    catch (const std::exception& e)
    {
        std::cerr << "[Control] Exception: " << e.what() << std::endl;
        std::string error = "Internal server error";
        send(fd, error.c_str(), error.size(), 0);
    }
}