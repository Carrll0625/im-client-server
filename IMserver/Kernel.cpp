#include "Kernel.h"
#include"../IMserver/mediator/TcpServerMediator.h"
Kernel* Kernel::pKernel = nullptr;
Kernel::Kernel() {
	m_pMediator = new TcpServerMediator;
	pKernel = this;
	
}
Kernel::~Kernel() {}

void Kernel::deal_BindProtArray()
{
	cout << __func__ << endl;
	m_protFunArr[DEF_PROT_DESIGNER_RQ - DEF_PROT_BASE] = &Kernel::deal_REGISTER_RQ;
	m_protFunArr[DEF_PROT_LOGIN_RQ - DEF_PROT_BASE] = &Kernel::deal_PROT_LOGIN_RQ;
	m_protFunArr[DEF_PROT_USERLINEOFF - DEF_PROT_BASE] = &Kernel::deal_PROT_USERLINEOFF;
	m_protFunArr[DEF_PROT_CHAT_INFO_RQ - DEF_PROT_BASE] = &Kernel::deal_PROT_CHAT_INFO_RQ;
	m_protFunArr[DEF_PROT_ADD_FRIEND_RQ - DEF_PROT_BASE] = &Kernel::deal_PROT_ADD_FRIEND_RQ;
	m_protFunArr[DEF_PROT_ADD_FRIEND_RS - DEF_PROT_BASE] = &Kernel::deal_PROT_ADD_FRIEND_RS;
}
//开启服务器
bool Kernel::OpenServer() {
	cout << __func__ << endl;
	deal_BindProtArray();
	if (!m_pMediator->openNet())
	{
		cout << "打开网络失败" << endl;
		return false;
	}
	//连接数据库
	char ip[] = "127.0.0.1";
	char name[] = "root";
	char pass[] = "Wyk20070625";
	char db[] = "20251018im";
	if (!m_sql.ConnectMySql(ip, name, pass, db))
	{
		cout << "连接数据库失败" << endl;
		return false;
	}
	else
	{
		cout << "连接数据库成功" << endl;
	}
	
	return true;
}

//结束服务器
void Kernel::CloseServer() {
	cout << __func__ << endl;
	m_pMediator->closeNet();
	m_sql.DisConnect();
}
//处理收发数据
void Kernel::dealData(char* data, int len, unsigned long to) {
	cout << __func__ << endl;
	protType type = *(protType*)data;
	//计算一下数组下标 下面便于进入函数地址数组中取地址
	int index = type - DEF_PROT_BASE;
	if (index >= 0 && index < PROT_COUNT)
	{
		DealFunc pf = m_protFunArr[index];
		if (pf)
		{
			(this->*pf)(data, len, to);
		}
		else
		{
			cout << "type2:" << type << endl;
		}
	}
	else
	{
		cout << "type1:" << type << endl;
	}
	
	delete[]data;
}


void Kernel::deal_REGISTER_RQ(char* data, int len, unsigned long to)
{
	cout << __func__ << endl;
	PROT_REGISTER_RQ* res = (PROT_REGISTER_RQ*)data;
	cout << "nick:" << res->nick << ",password:" << res->password << ",prottype:" << res->prottype << ",tel:" << res->tel << endl;
	PROT_REGISTER_RS rs;
	//校验昵称是否被注册过
	//1.1从数据库中根据昵称查询昵称
	list<string> listRes;
	char sql[1024] = "";
	sprintf_s(sql, "select nick from t_user where nick = '%s'", res->nick);
	if (!m_sql.SelectMySql(sql, 1, listRes))
	{
		cout << "查询数据库失败:" << sql << endl;
		return;
	}
	if (listRes.size() != 0)
	{
		//重复注册了
		rs.result = REGISTER_NICK_EXISTS;
		m_pMediator->sendData((char*)&rs, sizeof(rs), to);
		return;
	}
	//校验电话号是否被注册过
	sprintf_s(sql, "select tel from t_user where tel = '%s'", res->tel);
	if (!m_sql.SelectMySql(sql, 1, listRes))
	{
		cout << "查询数据库失败:" << sql << endl;
		return;
	}
	if (listRes.size() != 0)
	{
		//重复注册了
		rs.result = REGISTER_TEL_EXISTS;
		m_pMediator->sendData((char*)&rs, sizeof(rs), to);
		return;
	}
	//注册信息，把用户信息写入数据库
	sprintf_s(sql,"insert into t_user (nick,tel,pass,feeling,iconid) values('%s','%s','%s','饿啊','28')",res->nick,res->tel,res->password);
	if (!m_sql.UpdateMySql(sql))
	{
		cout << "插入数据库失败:" << sql << endl;
		return;
	}
	m_pMediator->sendData((char*)&rs, sizeof(rs), to);
}


void Kernel::deal_PROT_LOGIN_RQ(char* data, int len, unsigned long to)
{
	cout << __func__ << endl;
	PROT_LOGIN_RQ* res = (PROT_LOGIN_RQ*)data;
	cout << "password:" << res->password << ",prottype:" << res->prottype << ",tel:" << res->tel << endl;
	PROT_LOGIN_RS rs;
	//从数据库中，根据电话号查询密码
	list<string> listRes;
	char sql[1024] = "";
	sprintf_s(sql, "select nick from t_user where tel = '%s'", res->tel);
	if (!m_sql.SelectMySql(sql, 1, listRes))
	{
		cout << "查询数据库失败:" << sql << endl;
		return;
	}
	//判断查询结果
	if (listRes.size() == 0)
	{
		//电话号没有被注册
		rs.result = LOGIN_NOEXIST;
		m_pMediator->sendData((char*)&rs, sizeof(rs), to);
		return;
	}
	else
	{
		listRes.clear();
		sprintf_s(sql, "select pass,id from t_user where tel = '%s'", res->tel);
		m_sql.SelectMySql(sql, 2, listRes);
		//从查询结果中取出密码
		string passStr = listRes.front(); // 获取list第一个string元素
		listRes.pop_front();
		int Userid = stoi(listRes.front());
		listRes.pop_front();
		if (passStr==res->password)
		{
			//登录成功
			rs.result = LOGIN_SUCCESS;
			rs.userID = Userid;
			m_pMediator->sendData((char*)&rs, sizeof(rs), to);
			//保存当前用户id和socket
			m_mapUserIdToSocket[Userid] = to;
			//根据当前登录用户的id查询自己的和他人的信息
			getUserInfoAndFriendInfo(Userid);
		}
		else
		{
			rs.result = LOGIN_PASSERR;
			m_pMediator->sendData((char*)&rs, sizeof(rs), to);
			return;
		}
	}
	m_pMediator->sendData((char*)&rs, sizeof(rs), to);
}

//根据当前用户id查询自己的信息和好友的信息
void Kernel::getUserInfoAndFriendInfo(int id)
{
	cout << __func__ << endl;
	//先将自己的id发送过去
	
	PROT_FRIEND_INFO MyInfo;
	SelectUserInfo(id, &MyInfo);
	m_pMediator->sendData((char*)&MyInfo, sizeof(MyInfo), m_mapUserIdToSocket[id]);

	//查询好友列表
	list<string> listRes;
	char sql[1024] = "";
	sprintf_s(sql, "select A_id from t_friend where B_id = %d", id);
	if (!m_sql.SelectMySql(sql, 1, listRes))
	{
		cout << "查询好友列表失败:" << sql << endl;
		return;
	}
	else
	{
		while (listRes.size() > 0)
		{
			int FriendId = stoi(listRes.front());
			cout << "[好友循环] friendId=" << FriendId << endl;
			listRes.pop_front();
			PROT_FRIEND_INFO FriInfo; //每次循环新建
			SelectUserInfo(FriendId, &FriInfo);
			m_pMediator->sendData((char*)&FriInfo, sizeof(FriInfo), m_mapUserIdToSocket[id]);
			if (m_mapUserIdToSocket.count(FriendId) != 0)
			{
				m_pMediator->sendData((char*)&MyInfo, sizeof(MyInfo), m_mapUserIdToSocket[FriendId]);
			}
		}
	}
	
}

//查询信息函数
void Kernel::SelectUserInfo(int id, PROT_FRIEND_INFO* Info)
{
	cout << __func__ << endl;
	//先查询一下这个Id是否在m_map这个列表中 如果在列表中则说明其在线 没有则说明下线
	Info->userid = id;
	if (m_mapUserIdToSocket.count(id)!=0)
	{
		Info->status = STATUS_ONLINE;
	}
	else
	{
		Info->status = STATUS_NOONLINE;
	}
	list<string> listRes;
	char sql[1024] = "";
	sprintf_s(sql, "select nick,feeling,iconid from t_user where id = %d", id);
	if (!m_sql.SelectMySql(sql, 3, listRes))
	{
		cout << "查寻id信息库失败:" << sql << endl;
		return;
	}
	else
	{
		if (listRes.empty()) return;
		strcpy_s(Info->nick, sizeof(Info->nick),listRes.front().c_str());
		listRes.pop_front();

		if (listRes.empty()) return;
		strcpy_s(Info->feeling,sizeof(Info->feeling) ,listRes.front().c_str());
		listRes.pop_front();

		if (listRes.empty()) return;
		Info->userimgid = stoi(listRes.front());
		listRes.pop_front();
	}
}
//处理用户下线请求协议
void  Kernel::deal_PROT_USERLINEOFF(char* data, int len, unsigned long to)
{
	cout << __func__ << endl;
	PROT_USERLINEOFF* rq=(PROT_USERLINEOFF*) data;
	//通知所有在线好友我下线了
	//根据用户id查询好友id列表
	//查询好友列表
	list<string> listRes;
	char sql[1024] = "";
	sprintf_s(sql, "select A_id from t_friend where B_id = %d", rq->userid);
	if (!m_sql.SelectMySql(sql, 1, listRes))
	{
		cout << "查询好友列表失败:" << sql << endl;
		return;
	}
	//遍历好友列表
	else {
		int FriendId = 0;
		PROT_FRIEND_INFO FriInfo;
		while (listRes.size() > 0)
		{

			FriendId = stoi(listRes.front());
			listRes.pop_front();

			if (m_mapUserIdToSocket.count(FriendId) != 0)
			{
				m_pMediator->sendData(data, len , m_mapUserIdToSocket[FriendId]);
			}

		}
		//回收下线用户资源（关闭socket,从map中移除无效节点）
			closesocket(m_mapUserIdToSocket[rq->userid]);
			auto it = m_mapUserIdToSocket.find(rq->userid);
			m_mapUserIdToSocket.erase(it);
	}
}


void Kernel::deal_PROT_CHAT_INFO_RQ(char* data, int len, unsigned long to)
{
	cout << __func__ << endl;
	PROT_CHAT_INFO_RQ* rq = (PROT_CHAT_INFO_RQ*)data;
	//查询当前请求的friid是否在线
	if (m_mapUserIdToSocket.count(rq->friid) > 0)
	{
		m_pMediator->sendData(data, len, m_mapUserIdToSocket[rq->friid]);
	}
	else
	{
		//将回复请求发送回去
		PROT_CHAT_INFO_RS rs;
		rs.result = RESPOND_FAIL;
		rs.userid = rq->friid;
		rs.friid = rq->userid;
		m_pMediator->sendData((char*)&rs, sizeof(rs), to);
	}
}

void Kernel::deal_PROT_ADD_FRIEND_RQ(char* data, int len, unsigned long to)
{
	cout << __func__ << endl;
	PROT_ADD_FRIEND_RQ* rq = (PROT_ADD_FRIEND_RQ*) data;
	//查询数据库是否存在这个id
	list<string> listRes;
	char sql[1024] = "";
	sprintf_s(sql, "select id from t_user where nick = '%s'", rq->frinick);
	if (!m_sql.SelectMySql(sql, 1, listRes))
	{
		cout << "查询好友列表失败:" << sql << endl;
		return;
	}
	else
	{
		//判断查询结果是否为空
		if (listRes.empty())
		{
			//查询结果为空 说明没有用户叫这个昵称 添加失败
			PROT_ADD_FRIEND_RS rs;
			rs.result = ADD_FRI_NOEXIST;
			strcpy_s(rs.usernick, sizeof(rs.usernick), rq->frinick);
			//把失败结果发送给A
			m_pMediator->sendData((char*)&rs, sizeof(rs), to);
		}
		else
		{
			int FriendId = stoi(listRes.front());
			listRes.pop_front();
			//判断是否在线
			if (m_mapUserIdToSocket.count(FriendId) > 0)
			{
				//在线，把添加好友请求发送给B
				m_pMediator->sendData(data,len, m_mapUserIdToSocket[FriendId]);
			}
			else
			{
				//不在线，就告诉A
				PROT_ADD_FRIEND_RS rs;
				rs.result = ADD_FRI_FRIOFF;
				strcpy_s(rs.usernick, sizeof(rs.usernick), rq->frinick);
				//把失败的结果发送给A
				m_pMediator->sendData((char*)&rs, sizeof(rs), to);
			}
		}
	}
	
}
void Kernel::deal_PROT_ADD_FRIEND_RS(char* data, int len, unsigned long to)
{
	cout << __func__ << endl;
	PROT_ADD_FRIEND_RS* rs = (PROT_ADD_FRIEND_RS*)data;
	cout << "rs->userid:" << " " << rs->userid;
	cout << "rs->friid:" << " " << rs->friid<<endl;
	if (rs->result == ADD_FRI_ACCEPT)
	{
		//将好友关系写入关系库
		char sql[1024] = "";
		sprintf_s(sql, "insert into t_friend values(%d,%d)", rs->friid,rs->userid);
		if (!m_sql.UpdateMySql(sql))
		{
			cout << "插入数据库失败:" << sql << endl;
			return;
		}
		sprintf_s(sql, "insert into t_friend values(%d,%d)", rs->userid, rs->friid);
		if (!m_sql.UpdateMySql(sql))
		{
			cout << "插入数据库失败:" << sql << endl;
			return;
		}
		getUserInfoAndFriendInfo(rs->friid);
	}
	if(m_mapUserIdToSocket.count(rs->friid)>0)
	m_pMediator->sendData(data,len,m_mapUserIdToSocket[rs->friid]);
}