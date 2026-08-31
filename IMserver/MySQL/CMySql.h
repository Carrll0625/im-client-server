#pragma once

#include <mysql.h>
#include <string>
#include <iostream>

#pragma comment(lib,"libmysql.lib")
//
#include <list>
using namespace std;

class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:                    //ip,用户名,密码，数据库，端口号
    bool  ConnectMySql(char *host,char *user,char *pass,char *db,short nport = 3306);
    void  DisConnect();
	// 返回值代表查询语句是否执行成功
    bool  SelectMySql(char* szSql/*查询要执行的sql语句*/,int nColumn/*查询的列的个数*/ ,list<string>& lstStr/*sql语句的查询结果*/);
	// 获得数据库中的表
    bool GetTables(char* szSql,list<string>& lstStr);
    // 更新：删除、插入、修改，返回值代表sql语句是否执行成功
    bool  UpdateMySql(char* szSql);
 
private:
    MYSQL *m_sock;   
	MYSQL_RES *m_results;   
	MYSQL_ROW m_record; 
   
};

