#ifndef CONTROL_H
#define CONTROL_H
#include<jsoncpp/json/json.h>
#include "View.h"
#include <string>
#include <map>

class Control
{
public:
	Control();
	~Control(){};
	void process(int fd, std::string jsonStr);
private:
	std::map<int,View*> _map;
};
#endif
