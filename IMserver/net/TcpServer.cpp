#include "TcpServer.h"

TcpServer::TcpServer(TcpServerMediator* p_TcpServerMediator) {
	m_pMediator = p_TcpServerMediator;
}
TcpServer::~TcpServer() {}
//网络初始化
bool TcpServer::InitNet() {
	cout << "TcpServer::" << __func__ << endl;
	//1.加载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data = {};
	int err=WSAStartup(version, &data);
	if (err != 0)
	{
		cout << "WSAStartup fail" << endl;
		return false;
	}
	else
	{
		cout << "WSAStartup successed" << endl;
	}
	//2.创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
	{
		cout << "socket fail:" <<WSAGetLastError() <<endl;
		return false;
	}
	else
	{
		cout << "socket successed" << endl;
	}
	//3.绑定端口和ip
	sockaddr_in addr;
	addr.sin_port = htons(TCP_PORT);
	addr.sin_addr.S_un.S_addr = ADDR_ANY;
	addr.sin_family = AF_INET;
	err=bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (err == 0)
	{
		cout << "bind successed" << endl;
	}
	else
	{
		cout << "bind fail:" << WSAGetLastError() << endl;
		return false;
	}
	//4.监听
	err=listen(m_sock, 100);
	if (err != 0)
	{
		cout << "listen fail:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "listen successed" << endl;
	}
	m_handle=(HANDLE)_beginthreadex(nullptr, 0, &AcceptThread, this, 0, nullptr);
	return true;
}
unsigned __stdcall TcpServer::AcceptThread(void* lpVoid)
{
	TcpServer*  pthis = (TcpServer*) lpVoid;
	sockaddr_in add = {};
	int Size = sizeof(add);
	SOCKET sock = INVALID_SOCKET;
	HANDLE handle = nullptr;
	unsigned threadId = 0;
	while (pthis->m_bRunning)
	{
		
		sock = accept(pthis->m_sock, (sockaddr*)&add, &Size);

		if (sock == INVALID_SOCKET)
		{
			cout << "accept fail" << WSAGetLastError() << endl;
			return false;
		}
		else
		{
			cout << "accept successed" << "ip:" << inet_ntoa(add.sin_addr) << endl;
			//给每个连接成功的客户端创建一个接收数据的线程
			handle = (HANDLE)_beginthreadex(nullptr, 0, &recvThread, pthis, 0, &threadId);

			//保存线程id和socket一对一的关系
			pthis->m_mapThreadIdToSocket[threadId] = sock;

			//保存线程句柄
			pthis->m_listHandle.push_back(handle);
		}
	}
	return 1;
}
unsigned __stdcall TcpServer::recvThread(void* lpVoid)
{
	TcpServer* pthis = (TcpServer*)lpVoid;
	pthis->recvData();
	return 1;
}
//关闭网络
void TcpServer::unInitNet() {
	cout << "TcpServer::" << __func__ << endl;
	//回收线程资源
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
	HANDLE handle = nullptr;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();)
	{
		handle = *m_listHandle.begin();
		if (handle)
		{
			m_bRunning = false;
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 5000))
			{
				//强制杀死线程，不能直接杀死线程，不然会产生死锁问题
				TerminateThread(handle, -1/*退出码*/);
			}
			// 2.关闭句柄
			CloseHandle(handle);
			handle = nullptr;
		}
		ite = m_listHandle.erase(ite);
	}
	//关闭套接字
	if (!m_sock)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
	SOCKET socket = INVALID_SOCKET;
	for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();)
	{
		socket = ite->second;
		if (socket != INVALID_SOCKET)
		{
			closesocket(socket);
			socket = INVALID_SOCKET;
		}
		ite=m_mapThreadIdToSocket.erase(ite);
	}
	//卸载库
	WSACleanup();
}
//发送数据
//data:发送的数据内容
//len:数据长度 
//to:数据发送给谁
//UDP协议中决定发送给谁的是IP，IP是u_long类型
//TCP协议中决定发送给谁的是SOCKET，是u_int类型
bool TcpServer::sendData(char* data, int len, unsigned long to) {
	cout << "TcpServer::" << __func__ << endl;
	//检验数据合法性
	if (!data || len < 0)
	{
		cout << "data or len error" << endl;
		return false;
	}
	//先发送数据长度
	int sendnum = send(to, (char*)&len, sizeof(int),0);
	if (sendnum == 0)
	{
		cout << "sendnum fail:" << WSAGetLastError() << endl;
		return false;
	}
	sendnum=send(to,data,len,0);
	if (sendnum == 0)
	{
		cout << "sendnum fail:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "sendnum successed" << endl;
	}
	return true;
}
//接受数据
void TcpServer::recvData() {
	cout << "TcpServer::" << __func__ << endl;
	//避免计算机性能高 时间片切换过快导致accept线程函数中的连接数据没有全部存入就切换 在这里sleep一秒等待
	Sleep(1000);
	//接收数据的长度
	int packlen = 0;
	//数据接收的大小
	int offset = 0;
	//从map中取出当前线程对应的socket
	unsigned threadId = GetCurrentThreadId();
	SOCKET sock = INVALID_SOCKET;
	if (m_mapThreadIdToSocket.count(threadId) > 0)
	{
		//找到对应值
		sock = m_mapThreadIdToSocket[threadId];
	}
	else
	{
		cout << "sock error" << endl;
		return;
	}
	while (m_bRunning)
	{
		offset = 0;
		int recvNum = recv(sock, (char*)&packlen, sizeof(int), 0);
		if (recvNum > 0)
		{


			//创建一个临时接收的空间

			char* buf = new char[packlen];

			while (packlen > 0)
			{
				recvNum = recv(sock, buf + offset, packlen, 0);
				if (recvNum == SOCKET_ERROR)
				{
					cout << "recvnum fail:" << WSAGetLastError() << endl;
					break;
				}
				else
				{
					offset += recvNum;
					packlen -= recvNum;
				}

			}
			//TODO:把新空间的数据传给中介者
			m_pMediator->transmitData(buf, offset, sock);

		}
		else
		{
			cout << "recvnum fail:" << WSAGetLastError() << endl;
			break;
		}
	}
}