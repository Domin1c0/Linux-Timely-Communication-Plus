#include<Register.h>
#include<iostream>
#include <string>
#include <jsoncpp/json/json.h>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include"Register.h"
#include"MysqlWrapper.h"

extern MysqlWrapper Mysql_server;

void Register::process(int fd, std::string &json) 
{
    _fd = fd;

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(json, root)) 
    {
        throw std::runtime_error("Failed to parse JSON in Register::process");
    }

    std::string name = root["name"].asString();
    std::string pw = root["pw"].asString();

    try {
        MysqlWrapper db("127.0.0.1", "root", "password", "chat");

        // 检查用户名是否存在
        std::string check_sql = "SELECT * FROM user WHERE name='" + name + "';";
        MYSQL_RES* res = db.query(check_sql);

        if (mysql_fetch_row(res)) 
        {
            _responseStr = "register fail! username exists.";
        } 
        else 
        {
            // 插入新用户
            std::string insert_sql = "INSERT INTO user(name, passwd) VALUES('" + name + "', '" + pw + "');";
            db.execute(insert_sql);
            _responseStr = "register success!";
        }
        mysql_free_result(res);

    } 
    catch (const std::exception& e) 
    {
        _responseStr = std::string("Database error: ") + e.what();
    }
}

void Register::response()
{
    if (send(_fd, _responseStr.c_str(), _responseStr.size(), 0) == -1)
    {
        std::cerr << "send failed: " << strerror(errno) << std::endl;
    }
}