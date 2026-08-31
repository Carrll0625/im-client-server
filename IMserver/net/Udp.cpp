#include"Udp.h"
#include"../mediator/UdpMediator.h"

Udp::Udp(UdpMediator* pUdpMediator)
{
	m_pMediator = pUdpMediator;
}
Udp::~Udp() {

}
//网络初始化
bool Udp::InitNet() {
	cout<<"Udp::" << __func__ << endl;
	//1.记载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data = {};
	int err=WSAStartup(version, &data);
	if (err != 0)
	{
		cout << "WSAStartup fail" << endl;
		return false;
	}
	if (data.wVersion != version)
	{
		cout << "Version error" << endl;
		return false;
	}
	else
	{
		cout << "WSAStartup" << endl;
	}
	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		cout << "create socket fail" << endl;
		return false;
	}
	else
	{
		cout << "socket succeed" << endl;
	}
	//3.绑定端口和ip
	sockaddr_in addr;
	addr.sin_port = htons(UDP_PORT);
	addr.sin_addr.S_un.S_addr = ADDR_ANY;
	addr.sin_family = AF_INET;
	err=bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (err != 0)
	{
		cout << "bind fail" << endl;
		return false;
	}
	else
	{
		cout << "bind succeed" << endl;
	}
	//创建一个接受数据的线程，线程解决同时问题
	//CreateThread()和ExistThread()是一对，如果在线程中使用了c++运行时库的函数（例如strcpy），
	//这个函数申请的空间不会释放，ExistThread()也不会回收空间，就会造成内存泄露
	//_beginthreadex()和_endthreadex()是一对，_endthreadex()会先回收空间，再调用ExistThread()
	m_handle=(HANDLE)_beginthreadex(nullptr/*线程安全等级，nullptr为默认安全等级*/, 0/*堆栈大小，0为默认大小 1m*/, &recvThread/*线程函数地址*/,this/*线程函数的参数列表*/,0/*初始化标志位，0代表创建即运行*/,nullptr/*线程id，操作系统分配的，输出参数，不需要就不接*/);
	return true;
	
}

unsigned __stdcall Udp::recvThread(void* lpVoid)
{
	Udp* pthis = (Udp*)lpVoid;
	pthis->recvData();
	return 1;
}
//关闭网络
void Udp::unInitNet() {
	cout << "Udp::" << __func__ << endl;
	//回收线程资源
	//创建线程时，操作系统会给每个线程分配，线程id(自动回收)，句柄，内核对象
	// 1.结束线程函数
	if (m_handle)
	{
		m_bRunning = false;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 5000))
		{
			//强制杀死线程，不能直接杀死线程，不然会产生死锁问题
			TerminateThread(m_handle, -1/*退出码*/);
		}
		// 2.关闭句柄

		
		CloseHandle(m_handle);
		m_handle = nullptr;
		
	}
	
	//关闭套接字（检测一下m_sock是否为空 为空不用卸载）
	if (m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	//卸载库
	WSACleanup();
}
//接收数据
//data:发送的数据内容
//len:数据长度 
//to:数据发送给谁
//UDP协议中决定发送给谁的是IP，IP是u_long类型
//TCP协议中决定发送给谁的是SOCKET，是u_int类型
bool Udp::sendData(char* data, int len, unsigned long to) {
	cout << "Udp::" << __func__ << endl;
	//校验数据合法性
	if (!data || len <= 0)
	{
		cout << "data error" << endl;
		return false;
	}
	//发送数据
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_PORT);
	addr.sin_addr.S_un.S_addr = to;
	int err = sendto(m_sock, data, strlen(data), 0, (sockaddr*)&addr, sizeof(addr));
	if (err == 0)
	{
		cout << "sendto error" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "sendto successed"<< endl;
	}
	return true;
}
//发送数据
void Udp::recvData() {
	cout << "Udp::" << __func__ << endl;
	char recvbuf[4096];
	sockaddr_in addr;
	addr.sin_port = htons(UDP_PORT);
	addr.sin_family = AF_INET;
	int size = sizeof(addr);
	while (true)
	{
		int recvNum=recvfrom(m_sock, recvbuf, sizeof(recvbuf), 0, (sockaddr*)&addr, &size);
		if (recvNum > 0)
		{
			//接受数据成功，申请一个新的空间
			char* buf = new char[recvNum+1];
			//把接受的数据拷贝到一个新的空间中
			memcpy(buf, recvbuf, recvNum);
			buf[recvNum] = '\0';
			//TODO:把新空间的数据传给中介者
			m_pMediator->transmitData(buf, recvNum, m_sock);

		}
		else
		{
			//传输失败
			cout << "recvfrom error:" << WSAGetLastError() << endl;
			break;
		}
	}
	
}