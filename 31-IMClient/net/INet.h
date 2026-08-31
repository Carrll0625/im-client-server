#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>
#include<iostream>
#include<process.h>
#include"def.h"
#include"../mediator/INetMediator.h"
#pragma comment(lib,"Ws2_32.lib")
class INetMediator;
using namespace std;
class INet {
public:

	 INet():m_sock(INVALID_SOCKET), m_bRunning(true), m_handle(nullptr), m_pMediator(nullptr){}
	virtual ~INet() {}
	//网络初始化
	virtual bool InitNet ()=0;
	//关闭网络
	virtual void unInitNet() = 0;
	//接收数据
	//data:发送的数据内容
	//len:数据长度 
	//to:数据发送给谁
	//UDP协议中决定发送给谁的是IP，IP是u_long类型
	//TCP协议中决定发送给谁的是SOCKET，是u_int类型
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//发送数据
	virtual void recvData()=0;
protected:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
	INetMediator* m_pMediator;
};