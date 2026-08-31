#include<iostream>
#include"mediator/UdpMediator.h"
#include"mediator/TcpClientMediator.h"
#include"mediator/TcpServerMediator.h"
#include"Kernel.h"
using namespace std;
int main()
{
//-------------------------------------------------------测试代码---------------------------------------------------


	Kernel ker;
	if (!ker.OpenServer())
	{
		cout << "服务器打开失败" << endl;
		ker.CloseServer();
		return 1;
	}




	while (true)
	{
		cout << "server is running"<<endl;
		Sleep(50000);
	}

	ker.CloseServer();

	return 0;
}