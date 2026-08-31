#include"TcpClientMediator.h"
#include"../net/TcpClient.h"


TcpClientMediator::TcpClientMediator() {
	m_pINet = new TcpClient(this);
}
TcpClientMediator::~TcpClientMediator() {
	if (m_pINet)
	{
		m_pINet = nullptr;
		delete m_pINet;
	}
}
//打开网络
bool TcpClientMediator::openNet() {
	return m_pINet->InitNet();
}
//关闭网络
void TcpClientMediator::closeNet() {
	m_pINet->unInitNet();
}
//接收数据
//data:发送的数据内容
//len:数据长度 
//to:数据发送给谁
//UDP协议中决定发送给谁的是IP，IP是u_long类型
//TCP协议中决定发送给谁的是SOCKET，是u_int类型
bool TcpClientMediator::sendData(char* data, int len, unsigned long to) {
	return m_pINet->sendData(data, len, to);
}
//转发数据
void TcpClientMediator::transmitData(char* data, int len, unsigned long to) {
	//-------------------------------------------测试代码，打印接受的数据------------------------------------------------
	cout << "TcpClientMediator recv:" << data << " " << "len:" << to << endl;
}