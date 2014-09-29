#include <iostream>
#include <cstdlib>
#include <string>
#include "mysql/mysql.h"   //安装的mysql的头文件所在的位置

using namespace std;

string host="127.0.0.1";
string usr="root";
string pwd="941126";
string dbname="DDZ";
unsigned int port=3306;
int status;
MYSQL *mysql;
MYSQL_RES *result;
MYSQL_ROW row;

bool sqlConnect()
{
	mysql=mysql_init(0);
	mysql_set_character_set(mysql,"utf-8");
	if(mysql_real_connect(mysql,host.c_str(),usr.c_str(),pwd.c_str(),dbname.c_str(),port,NULL,CLIENT_FOUND_ROWS)==NULL){
	return false;
	}
	else{
	return true;
	}
}


MYSQL_ROW  sqlSelect(string sql)
{
	status=mysql_query(mysql,sql.c_str());
	if(status !=0 ){
	return row;
	}
	result=mysql_store_result(mysql);
	row=mysql_fetch_row(result);
	return row;
}
bool  sqlInsert(string sql)
{
	status=mysql_query(mysql,sql.c_str());
	if(status !=0 ){
	return false;
	}
	return true;
}
int  sqlUpdate(string sql)
{
	status=mysql_query(mysql,sql.c_str());
	unsigned long affectNum;
	affectNum=mysql_affected_rows(mysql);
	if(status !=0 ){
	return -1;
	}
	else{
	return affectNum;
	}
}
bool  sqlDelete(string sql)
{
	status=mysql_query(mysql,sql.c_str());
	if(status !=0 ){
	return false;
	}
	return true;
}

/*使用方法：
int main()
{
	sqlConnect();//sqlConnect用来检测数据库状态,正确返回true,否则返回false,在执行sql语句前必须执行此函数
	MYSQL_ROW sql1=   sqlSelect("SELECT * FROM usrList");//sqlSelect用来处理select语句，正确返回MYSQL_ROW对象，通过MYSQL_ROW['列名']来访问对应列字符串，错误返回-1
	cout<<sql1[1]<<endl;
	if(sqlInsert("INSERT INTO usrList (usrName, usrPassword) VALUES ('lalala', '123')"))//sqlInsert用来处理insert语句，正确返回true,否则返回false
	cout<<"Insert OK!"<<endl;
	int sql2=sqlUpdate("UPDATE usrList SET usrPassword = '456' WHERE usrName = 'lalala' "))//sqlUpdate用来处理update语句，正确返回影响行数,否则返回0
	cout<<"Update OK!"<<endl;
	if(sqlDelete("DELETE FROM usrList WHERE usrName = 'lalala' "))//sqlDelete用来处理insert语句，正确返回true,否则返回false
	cout<<"Delete OK!"<<endl;
}
*/



//g++ -o server server.cpp -lmysqlclient -I/usr/include/mysql/ -L/usr/lib/mysql