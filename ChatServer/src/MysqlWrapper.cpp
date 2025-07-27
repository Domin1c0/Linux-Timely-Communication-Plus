#include"MysqlWrapper.h"
#include"Global.h"
#include<iostream>
#include<stdexcept>

MysqlWrapper::MysqlWrapper(const std::string& host, const std::string& user,
            const std::string& password, const std::string& db, unsigned int port)
{
    _conn = mysql_init(nullptr);
    if(!_conn)
    {
        throw std::runtime_error("mysql_init failed");
    }

    if(!mysql_real_connect(_conn, host.c_str(), user.c_str(), 
            password.c_str(), db.c_str(), port, nullptr, 0))
    {
        std::string error_msg = mysql_error(_conn);
        mysql_close(_conn);
        throw std::runtime_error("mysql_real_connect failed: " + error_msg);
    }
}

MysqlWrapper::~MysqlWrapper() {
    if (_conn) {
        mysql_close(_conn);
    }
}

bool MysqlWrapper::execute(const std::string& sql) {
    if (mysql_query(_conn, sql.c_str())) {
        throw std::runtime_error("MySQL execute error: " + std::string(mysql_error(_conn)));
    }
    return true;
}

MYSQL_RES* MysqlWrapper::query(const std::string& sql) {
    if (mysql_query(_conn, sql.c_str())) {
        throw std::runtime_error("MySQL query error: " + std::string(mysql_error(_conn)));
    }
    return mysql_store_result(_conn);
}

