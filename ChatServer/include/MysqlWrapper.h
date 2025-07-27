#ifndef MYSQL_WRAPPER_H
#define MYSQL_WRAPPER_H

#include<mysql/mysql.h>
#include<string>
#include<stdexcept>

class MysqlWrapper
{
    public:
         MysqlWrapper(const std::string& host, const std::string& user,
            const std::string& password, const std::string& db, unsigned int port = 3306);
        ~MysqlWrapper();

        bool execute(const std::string& sql);  // 执行 INSERT / UPDATE / DELETE
        MYSQL_RES* query(const std::string& sql); // 执行 SELECT，返回结果集
    private:
        MYSQL* _conn;
};

#endif