#pragma once
#include"../net/INet.h"
class INet;
class INetMediator {
public:

	INetMediator() { m_pINet = nullptr; }
	virtual ~INetMediator() {}
	//打开网络
	virtual bool openNet() = 0;
	//关闭网络
	virtual void closeNet() = 0;
	//接收数据
	//data:发送的数据内容
	//len:数据长度 
	//to:数据发送给谁
	//UDP协议中决定发送给谁的是IP，IP是u_long类型
	//TCP协议中决定发送给谁的是SOCKET，是u_int类型
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//转发数据
	virtual void transmitData(char* data, int len, unsigned long to) = 0;
protected:
	INet* m_pINet;
};