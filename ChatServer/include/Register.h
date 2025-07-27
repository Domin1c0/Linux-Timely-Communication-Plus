#ifndef REGISTER_H
#define REGISTER_H
#include"View.h"
#include "MysqlWrapper.h"

class Register : public View
{
    public:
        Register(MysqlWrapper* db) : _db(db) {}
        void process(int fd, std::string &json) override;
        void response() override;
    private:
        int _fd;
        std::string _responseStr;
        MysqlWrapper* _db;
};


#endif