#ifndef LOGIN_H
#define LOGIN_H

#include<string>
#include"View.h"
#include"MysqlWrapper.h"

class Login : public View
{
public:
    explicit Login(MysqlWrapper* db);
    void process(int fd, std::string &json) override;
    void response() override;
private:
    int _fd;
    std::string _str;
    std::string _name;
    MysqlWrapper* _db;
};

#endif
