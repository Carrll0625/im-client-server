#pragma once
#include "INetMediator.h"
class UdpMediator :public INetMediator {
public:

	UdpMediator();
	~UdpMediator();
	//打开网络
	bool openNet();
	//关闭网络
	void closeNet();
	//接收数据
	//data:发送的数据内容
	//len:数据长度 
	//to:数据发送给谁
	//UDP协议中决定发送给谁的是IP，IP是u_long类型
	//TCP协议中决定发送给谁的是SOCKET，是u_int类型
	bool sendData(char* data, int len, unsigned long to);
	//转发数据
	void transmitData(char* data, int len, unsigned long to);
};