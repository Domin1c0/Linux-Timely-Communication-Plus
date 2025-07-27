#ifndef CONTROL_H
#define CONTROL_H
#include<jsoncpp/json/json.h>
#include "View.h"
#include"Public.h"
#include "MysqlWrapper.h"
#include "Register.h"
#include "Login.h"
#include "Talk_one.h"
#include "Talk_group.h"
#include "Exit.h"
#include "Get_List.h"
#include <map>
#include <memory>
#include <string>

class Control
{
public:
	Control(MysqlWrapper* db, std::map<int, struct event*>& eventMap);
	~Control();

	void process(int fd, std::string &json);
private:
	std::map<int,View*> _map;
};
#endif
