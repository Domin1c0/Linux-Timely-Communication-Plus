#include <event.h>
#include <iostream>
#include <map>
#include "MysqlWrapper.h"
#include "Control.h"
#include "TcpServer.h"
#include"Global.h"

MysqlWrapper* Mysql_server = nullptr;
Control* control_server = nullptr;

int main()
{
    try {
        static MysqlWrapper db("127.0.0.1", "dbeaveruser", "62595889", "chat", 3306);
        Mysql_server = &db;

        static std::map<int, struct event*> eventMap;
        static Control control(&db, eventMap);
        control_server = &control;

        TcpServer server("0.0.0.0", 5555, 4, control_server);
        server.run();
    } 
    catch(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

