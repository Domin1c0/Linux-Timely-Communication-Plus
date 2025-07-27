#ifndef TALK_GROUP
#define TALK_GROUP
#include "View.h"
#include"MysqlWrapper.h"
#include<vector>

class Talk_group : public View
{
public:
    Talk_group(MysqlWrapper* db) : _db(db){}
	void process(int fd, std::string &json) override;
	void response() override;
private:
	int _fd;
	std::string _responseStr;
    MysqlWrapper* _db;

    std::vector<std::string> splitRecipients(const std::string& str, char delimiter);
};

#endif