#include"MysqlWrapper.h"
#include<iostream>
#include<mysql/mysql.h>
//#include<TcpServer.h>

using namespace std;

int main()
{
    // try
    // {
    //     TcpServer server("127.0.0.1", 8888, 4);
    //     server.run();
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << "Erroe: " << e.what() << std::endl;
    // }
    try {
        // 根据你的数据库配置修改以下参数
        MysqlWrapper db("127.0.0.1", "dbeaveruser", "62595889", "chat", 3306);
        cout << "Connected to MySQL successfully!" << endl;

        // 测试查询
        string query = "SELECT name, passwd FROM user;";
        MYSQL_RES* res = db.query(query);
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                cout << "User: " << row[0] << ", Password: " << row[1] << endl;
            }
            mysql_free_result(res);
        } else {
            cerr << "Query failed!" << endl;
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}