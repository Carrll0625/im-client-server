#include"TcpClient.h"

TcpClient::TcpClient(TcpClientMediator* p_TcpClientMediator) {
	m_pMediator = p_TcpClientMediator;
}
TcpClient::~TcpClient() {}
//网络初始化
bool TcpClient::InitNet() {
	cout << "TcpClient::" << __func__ << endl;
	//1.加载库
	WORD version = MAKEWORD(2, 2);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
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
		cout << "socket fail:" << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout << "socket successed" << endl;
	}
	sockaddr_in addr;
	addr.sin_port = htons(TCP_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.109");
	addr.sin_family = AF_INET;
	err = connect(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (err == 0)
	{
		cout << "connect succeessed" << endl;
	}
	else
	{
		cout << "connect fail:" << WSAGetLastError() << endl;
	}
	m_handle=(HANDLE)_beginthreadex(nullptr, 0, &recvThread, this, 0, nullptr);
	return true;
} 
unsigned __stdcall TcpClient::recvThread(void* lpVoid)
{
	TcpClient* pthis = (TcpClient*)lpVoid;
	pthis->recvData();
	return 1;
}
//关闭网络
void TcpClient::unInitNet() {
	cout << "TcpClient::" << __func__ << endl;
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
	//关闭套接字
	if (!m_sock)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
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
bool TcpClient::sendData(char* data, int len, unsigned long to) {
	cout << "TcpClient::" << __func__ << endl;
	//1.判断数据合法性
	if (!data || len < 0)
	{
		cout << "data or len error" << endl;
		return false;
	}
	//采取先发送数据长度，再发送数据大小
	int lenthNum=send(m_sock, (char*)&len, sizeof(int), 0);
	if (lenthNum == 0)
	{
		cout << "sendLenth fail:" << WSAGetLastError() << endl;
		return false;
	}
	int sendnum=send(m_sock, data, len, 0);
	if (sendnum == 0)
	{
		cout << "sendnum fail:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
//接受数据
void TcpClient::recvData() {
	cout << "TcpClient::" << __func__ << endl;
	//接收数据的长度
	int packlen = 0;
	//数据接收的大小
	int offset = 0;
	while (m_bRunning)
	{
		offset = 0;
		int recvNum = recv(m_sock, (char*)&packlen, sizeof(int), 0);
		if (recvNum > 0)
		{


			//创建一个临时接收的空间

			char* buf = new char[packlen];

            while (packlen > 0)
			{
				recvNum = recv(m_sock, buf + offset, packlen, 0);
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
			m_pMediator->transmitData(buf, offset, m_sock);

		}
		else
		{
			cout << "recvnum fail:" << WSAGetLastError() << endl;
			break;
		}
	}
}
