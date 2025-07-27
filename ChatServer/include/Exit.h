#ifndef EXIT_H
#define EXIT_H
#include "View.h"
#include "MysqlWrapper.h"
#include <map>
#include<string>
#include <event.h>

class Exit : public View
{
public:
    Exit(MysqlWrapper* db, std::map<int, struct event*>& eventMap)
        : _db(db), _eventMap(eventMap) {}
	void process(int fd, std::string &json);
	void response();
private:
	int _fd;
	std::string _responseStr;
    MysqlWrapper* _db;
    std::map<int, struct event*>& _eventMap; // 引用主线程事件 map
};

#endif