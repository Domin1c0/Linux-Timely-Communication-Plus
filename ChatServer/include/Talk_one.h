#ifndef TALK_ONE_H
#define TALK_ONE_H

#include"MysqlWrapper.h"
#include "View.h"
#include <string>

class Talk_one : public View {
public:
    Talk_one(MysqlWrapper* db) : _db(db){}
    void process(int fd, std::string& json) override;
    void response() override;

private:
    int _fd;
    std::string _responseStr;
    MysqlWrapper* _db; 
};

#endif
