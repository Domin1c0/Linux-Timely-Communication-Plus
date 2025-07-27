#include"Login.h"
#include<sys/socket.h>
#include<jsoncpp/json/json.h>
#include<iostream>
#include<sstream>
#include<stdexcept>

Login::Login(MysqlWrapper* db) : _db(db) {}

void Login::process(int fd, std::string &json)
{
    _fd = fd;

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errs;

    //JSON 解析
    std::istringstream s(json);
    if(!Json::parseFromStream(reader, s, &root, &errs))
    {
        throw std::runtime_error("JSON parse failed: " + errs);
    }

    std::string name = root["name"].asString();
    std::string pw = root["pw"].asString();
    _name = name;

    //检查用户是否存在且密码正确
    std::ostringstream checkUser;
    checkUser << "SELECT * FROM user WHERE name='" << name << "' AND passwd='" << pw << "';";
    MYSQL_RES* res = _db->query(checkUser.str());

    if (!mysql_fetch_row(res)) {
        _responseStr = "login fail! (user not found or wrong password)";
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);

    //检查是否在线
    std::ostringstream checkOnline;
    checkOnline << "SELECT * FROM online WHERE name='" << name << "';";
    res = _db->query(checkOnline.str());

    if (mysql_fetch_row(res)) {
        _responseStr = "login fail! (user already online)";
        mysql_free_result(res);
        return;
    }
    mysql_free_result(res);

    //插入online表
    std::ostringstream insertOnline;
    insertOnline << "INSERT INTO online(fd, name) VALUES('" << fd << "', '" << name << "');";
    if (!_db->execute(insertOnline.str())) {
        throw std::runtime_error("Failed to insert into online table");
    }
}

void Login::response()
{

    if (send(_fd, _responseStr.c_str(), _responseStr.size(), 0) < 0) {
        std::cerr << "send error" << std::endl;
    }

    //查询离线消息
    std::ostringstream queryOffline;
    queryOffline << "SELECT message FROM offline WHERE name='" << _name << "';";
    MYSQL_RES* res = _db->query(queryOffline.str());

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        std::string msg = row[0];
        send(_fd, msg.c_str(), msg.size(), 0);
    }
    mysql_free_result(res);

    //删除离线消息
    std::ostringstream deleteOffline;
    deleteOffline << "DELETE FROM offline WHERE name='" << _name << "';";
    _db->execute(deleteOffline.str());
}