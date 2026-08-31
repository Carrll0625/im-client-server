#include"INet.h"
#include"../mediator/UdpMediator.h"
class Udp :public INet
{
public:
	Udp(UdpMediator* pUdpMediator);
	~Udp();
	//网络初始化
	 bool InitNet();
	//关闭网络
	 void unInitNet();
	//接收数据
	//data:发送的数据内容
	//len:数据长度 
	//to:数据发送给谁
	//UDP协议中决定发送给谁的是IP，IP是u_long类型
	//TCP协议中决定发送给谁的是SOCKET，是u_int类型
	bool sendData(char* data, int len, unsigned long to);
	//发送数据
	void recvData();

	//创建一个线程函数
	static unsigned __stdcall recvThread(void* lpVoid);
};