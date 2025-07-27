#include"Get_List.h"
#include "MysqlWrapper.h"
#include <mysql/mysql.h>
#include <sstream>
#include <iostream>
#include<sys/socket.h>

extern MysqlWrapper* db;  // 假设你有全局的数据库连接指针

void Get_List::process(int fd, std::string& json) {
    _fd = fd;
    _onlineUsers.clear();

    std::string sql = "SELECT name FROM online;";
    MYSQL_RES* res = db->query(sql);
    if (!res) {
        std::cerr << "Failed to query online users" << std::endl;
        _responseStr = "[]";  // 返回空列表
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        _onlineUsers.emplace_back(row[0]);
    }
    mysql_free_result(res);
}

void Get_List::response()
{
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < _onlineUsers.size(); ++i) {
        oss << "\"" << _onlineUsers[i] << "\"";
        if (i != _onlineUsers.size() - 1) oss << ",";
    }
    oss << "]";
    _responseStr = oss.str();
    send(_fd, _responseStr.c_str(), _responseStr.size(), 0);
}