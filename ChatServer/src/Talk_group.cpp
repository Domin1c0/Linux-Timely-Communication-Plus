#include "Talk_group.h"
#include <jsoncpp/json/json.h>
#include <sstream>
#include <sys/socket.h>
#include <iostream>

void Talk_group::process(int fd, std::string &json) {
    _fd = fd;
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(json, root)) {
        std::cerr << "JSON parse failed\n";
        _responseStr = "Invalid request format!";
        return;
    }

    std::string myname = root["name"].asString();
    std::string message = root["message"].asString();
    std::string hernames = root["hername"].asString(); // 群成员，以 ; 分隔

    // 解析群成员列表
    std::vector<std::string> recipients = splitRecipients(hernames, ';');
    if (recipients.empty()) {
        _responseStr = "No recipients found!";
        return;
    }

    // 群聊消息内容
    std::string fullMessage = "Group message from " + myname + ": " + message;

    // 遍历群成员
    for (const std::string& hername : recipients) {
        // 查询对方是否在线
        std::ostringstream queryOnline;
        queryOnline << "SELECT fd FROM online WHERE name='" << hername << "';";
        MYSQL_RES* res = _db->query(queryOnline.str());

        MYSQL_ROW row = mysql_fetch_row(res);
        if (row) {
            // 在线 -> 直接发送
            int ffd = std::stoi(row[0]);
            send(ffd, fullMessage.c_str(), fullMessage.size(), 0);
            _responseStr += "[Sent to " + hername + "]";
        } else {
            // 离线 -> 查询是否注册
            std::ostringstream queryUser;
            queryUser << "SELECT * FROM user WHERE name='" << hername << "';";
            MYSQL_RES* resUser = _db->query(queryUser.str());

            if (mysql_fetch_row(resUser)) {
                // 已注册 -> 存储离线消息
                std::ostringstream insertOffline;
                insertOffline << "INSERT INTO offline(name, message) VALUES('"
                              << hername << "', '" << fullMessage << "');";
                _db->execute(insertOffline.str());
                _responseStr += "[Offline message stored for " + hername + "]";
            } else {
                // 未注册用户
                _responseStr += "[Failed: user " + hername + " not found]";
            }
            mysql_free_result(resUser);
        }
        mysql_free_result(res);
    }
}

void Talk_group::response() {
    send(_fd, _responseStr.c_str(), _responseStr.size(), 0);
}

std::vector<std::string> Talk_group::splitRecipients(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delimiter)) {
        if (!token.empty()) result.push_back(token);
    }
    return result;
}
