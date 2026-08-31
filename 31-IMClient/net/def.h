#ifndef __DEF_H__
#define __DEF_H__

#define UDP_PORT 5555
#define TCP_PORT 6666
#define DEF_PROT_BASE 1000
#define DEF_PROT_DESIGNER_RQ (DEF_PROT_BASE+0)
#define DEF_PROT_DESIGNER_RS (DEF_PROT_BASE+1)
#define DEF_PROT_LOGIN_RQ       (DEF_PROT_BASE+2)
#define DEF_PROT_LOGIN_RS       (DEF_PROT_BASE+3)
#define DEF_PROT_FRIENDINFO     (DEF_PROT_BASE+4)
#define DEF_PROT_CHAT_INFO_RQ  (DEF_PROT_BASE+5)
#define DEF_PROT_CHAT_INFO_RS  (DEF_PROT_BASE+6)
#define DEF_PROT_ADD_FRIEND_RQ  (DEF_PROT_BASE+7)
#define DEF_PROT_ADD_FRIEND_RS  (DEF_PROT_BASE+8)
#define DEF_PROT_USERLINEOFF  (DEF_PROT_BASE+9)
#define REGISTER_SUCCESS 0
#define REGISTER_NICK_EXISTS 1
#define REGISTER_TEL_EXISTS 2


#define LOGIN_SUCCESS 0
#define LOGIN_NOEXIST 1
#define LOGIN_PASSERR 2

//在线状态
#define STATUS_ONLINE 0
#define STATUS_NOONLINE 1

//回复成功/失败
#define RESPOND_SUCC 0
#define RESPOND_FAIL 1
//输入内容字节最大长度 18k
#define MESSAGE_MAX_LEN 18*1024

#define ADD_FRI_ACCEPT 0
#define ADD_FRI_REJECT 1
#define ADD_FRI_FRIOFF 2
#define ADD_FRI_NOEXIST 3

using protType =unsigned int ;
#pragma pack(push,1)
struct PROT_REGISTER_RQ
{
    protType prottype;
    char nick[30];
    char tel[15];
    char password[20];
    PROT_REGISTER_RQ():prottype(DEF_PROT_DESIGNER_RQ),nick{0},tel{0},password{0}{}
};


struct PROT_REGISTER_RS
{
    protType prottype;
    int result;
    PROT_REGISTER_RS():prottype(DEF_PROT_DESIGNER_RS),result(REGISTER_SUCCESS){}
};

//登录请求协议
struct PROT_LOGIN_RQ
{
    protType prottype;
    char tel[15];
    char password[20];
    PROT_LOGIN_RQ():prottype(DEF_PROT_LOGIN_RQ)
      ,tel{0}
      ,password{0}
    {}

};




//登录回复协议

struct PROT_LOGIN_RS
{
    protType prottype;
    int userID;
    int result;
    PROT_LOGIN_RS():prottype(DEF_PROT_LOGIN_RS)
      ,userID(0)
      ,result(LOGIN_SUCCESS)
    {}
};

struct PROT_FRIEND_INFO
{
    protType prottype;
    int userid;
    int userimgid;
    int status;
    char nick[30];
    char feeling[100];
    PROT_FRIEND_INFO():
        prottype(DEF_PROT_FRIENDINFO),
        userid(0)
      ,userimgid(0)
      ,status(STATUS_ONLINE)
      ,nick{0}
      ,feeling{0}
    {}
};

//用户聊天请求协议
struct PROT_CHAT_INFO_RQ
{
    protType prottype;
    int userid;
    int friid;
    char message[MESSAGE_MAX_LEN] ;
    PROT_CHAT_INFO_RQ():prottype(DEF_PROT_CHAT_INFO_RQ),userid(0),friid(0),message{0}{}
};
//用户聊天回复协议
struct PROT_CHAT_INFO_RS
{
    protType prottype;
    int userid;
    int friid;
    int result;
    PROT_CHAT_INFO_RS():prottype(DEF_PROT_CHAT_INFO_RS),userid(0),friid(0),result(RESPOND_SUCC){}
};
//好友申请协议
struct PROT_ADD_FRIEND_RQ
{
    protType prottype;
    int userid;
    char usernick[30];
    char frinick[30];
    PROT_ADD_FRIEND_RQ():prottype(DEF_PROT_ADD_FRIEND_RQ),userid(0),usernick{0},frinick{0}{}
};
struct PROT_ADD_FRIEND_RS
{
    protType prottype;
    int result;
    int friid;
    char frinick[30];
    int userid;
    char usernick[30];
    PROT_ADD_FRIEND_RS():prottype(DEF_PROT_ADD_FRIEND_RS),result(ADD_FRI_ACCEPT),friid(0),frinick{0},userid(0),usernick{0}{}
};
//用户下线协议
struct PROT_USERLINEOFF
{
    protType prottye;
    int userid;//目标用户id
    PROT_USERLINEOFF():prottye(DEF_PROT_USERLINEOFF),userid(0){}
};
#pragma pack(pop)
#endif
