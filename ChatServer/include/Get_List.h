#ifndef GET_LIST_H
#define GET_LIST_H

#include "View.h"
#include <string>
#include <vector>

class Get_List : public View
{
public:
Get_List(MysqlWrapper* db): _db(db){}
    void process(int fd, std::string& json) override;
    void response() override;

private:
    int _fd;
    std::string _responseStr;
    std::vector<std::string> _onlineUsers;
    MysqlWrapper* _db;
};

#endif
