#include "Exit.h"
#include <jsoncpp/json/json.h>
#include <sstream>
#include <sys/socket.h>
#include <iostream>

void Exit::process(int fd, std::string& json)
{
    _fd = fd;
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(json, root)) {
        std::cerr << "[Exit] JSON parse failed" << std::endl;
       _responseStr = "exit fail: invalid json";
        return;
    }

    std::string name = root["name"].asString();

    // 从数据库删除在线用户记录
    std::ostringstream deleteQuery;
    deleteQuery << "DELETE FROM online WHERE fd='" << fd << "' AND name='" << name << "';";

    if (_db->execute(deleteQuery.str())) {
       _responseStr = name + " exit~";
        std::cout << "[Exit] User " << name << " removed from online" << std::endl;
    } else {
       _responseStr = "exit fail!";
        std::cerr << "[Exit] Failed to remove user: " << name << std::endl;
        return;
    }

    // 清理对应 fd 的 libevent 事件
    auto it = _eventMap.find(_fd);
    if (it != _eventMap.end()) {
        event_free(it->second);
        _eventMap.erase(it);
        std::cout << "[Exit] Event removed for fd: " << _fd << std::endl;
    }
}

void Exit::response()
{
    send(_fd,_responseStr.c_str(),_responseStr.size(), 0);
}
