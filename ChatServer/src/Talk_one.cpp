#include "Talk_one.h"
#include <jsoncpp/json/json.h>
#include <sstream>
#include <sys/socket.h>
#include <stdexcept>
#include <iostream>
#include <mysql/mysql.h>


void Talk_one::process(int fd, std::string &json) {
    _fd = fd;

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(json, root)) {
        _responseStr = "Invalid JSON format!";
        return;
    }

    std::string myname = root["name"].asString();
    std::string hername = root["hername"].asString();
    std::string message = root["message"].asString();

    // 1. 检查 hername 是否在线
    std::ostringstream queryOnline;
    queryOnline << "SELECT fd FROM online WHERE name='" << hername << "';";
    MYSQL_RES* res = _db->query(queryOnline.str());
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        // 在线，直接发消息
        int ffd = atoi(row[0]);
        std::string sendMsg = "your friend " + myname + ": " + message;
        send(ffd, sendMsg.c_str(), sendMsg.size(), 0);
        _responseStr = "message send!";
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);

    // 2. 检查 hername 是否存在
    std::ostringstream queryUser;
    queryUser << "SELECT * FROM user WHERE name='" << hername << "';";
    res = _db->query(queryUser.str());
    row = mysql_fetch_row(res);

    if (row) {
        // 用户存在，插入 offline 消息
        std::string offlineMsg = "offline message from " + myname + ": " + message;
        std::ostringstream insertOffline;
        insertOffline << "INSERT INTO offline (name, message) VALUES('"
                      << hername << "', '" << offlineMsg << "');";
        _db->execute(insertOffline.str());
        _responseStr = "offline message send!";
        mysql_free_result(res);
        return;
    }

    mysql_free_result(res);
    _responseStr = "message send fail!";
}

void Talk_one::response() {
    send(_fd, _responseStr.c_str(), _responseStr.size(), 0);
}
