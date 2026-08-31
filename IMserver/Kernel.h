#pragma once
#include<map>
#include<iostream>
#include"../IMserver/mediator/INetMediator.h"
#include"net/Inet.h"
#include"MySQL/CMySql.h"
using namespace std;

class Kernel
{
	
public:
	Kernel();
	~Kernel();

	//开启服务器
	bool OpenServer();

	//结束服务器
	void CloseServer();
	//处理收发数据
	void dealData(char* data, int len, unsigned long to);

	//处理用户注册请求协议
	void deal_REGISTER_RQ(char* data, int len, unsigned long to);

	//处理用户登录请求协议
	void deal_PROT_LOGIN_RQ(char* data, int len, unsigned long to);
	//处理用户下线请求协议
	void deal_PROT_USERLINEOFF(char* data, int len, unsigned long to);
	//处理用户聊天请求协议
	void deal_PROT_CHAT_INFO_RQ(char* data, int len, unsigned long to);
	//处理用户申请添加好友协议
	void deal_PROT_ADD_FRIEND_RQ(char* data, int len, unsigned long to);
	//处理用户申请添加好友回复
	void deal_PROT_ADD_FRIEND_RS(char* data, int len, unsigned long to);
	//绑定处理函数地址
	void deal_BindProtArray();

	//根据当前用户id查询自己的信息和好友的信息
	void getUserInfoAndFriendInfo(int id);

	void SelectUserInfo(int id, PROT_FRIEND_INFO* Info);



	static Kernel* pKernel;
private:
	INetMediator* m_pMediator;
	
	using DealFunc = void (Kernel::*) (char*, int, unsigned long);
	DealFunc m_protFunArr[PROT_COUNT];
	CMySql m_sql;
	map<int, SOCKET> m_mapUserIdToSocket;
};

