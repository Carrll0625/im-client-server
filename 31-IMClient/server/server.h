#ifndef __SERVER_H__
#define __SERVER_H__
//模拟服务端的文件
#include"../def/def.h"
#include<QDebug>
#include"kernel.h"
extern Kernel *pkernel;

#define RETURN_CLIENT_DATA(PROT_TYPE,PROT_OBJ) \
   { int len = sizeof(PROT_TYPE);\
\
    char *pbuf=new char[len];\
    memcpy(pbuf,&PROT_OBJ,len);\
    emit pkernel->signals_return_Client_Data(pbuf,len,0);\
}

inline void server_Deal_Reg_Info(const PROT_REGISTER_RQ &registerRq)
{
    qDebug()<<"服务端收到了客户端信息 :"<<" 昵称："<<registerRq.nick<<"电话："<<registerRq.tel<<"密码："<<registerRq.password;

    PROT_REGISTER_RS registerRs;
    registerRs.result=REGISTER_SUCCESS;

    int len = sizeof(PROT_REGISTER_RS);

    char *pbuf=new char[len];
    memcpy(pbuf,&registerRs,len);
    emit pkernel->signals_return_Client_Data(pbuf,len,0);


}

inline void server_Deal_Login_Info(const PROT_LOGIN_RQ &loginRq)
{
    qDebug()<<"服务端收到了客户端信息 :"<<"电话："<<loginRq.tel<<"密码："<<loginRq.password;

    PROT_LOGIN_RS loginRs;
    loginRs.result=LOGIN_SUCCESS;  //LOGIN_SUCCESS   LOGIN_NOEXIST   LOGIN_PASSERR
    loginRs.userID=5;

    RETURN_CLIENT_DATA(PROT_LOGIN_RS,loginRs)
    //如果登录成功，返回登录用户和所有朋友的信息

    if(loginRs.result==LOGIN_SUCCESS)
    {
        PROT_FRIEND_INFO myInfo;
        myInfo.userid=5;
        myInfo.userimgid=5;
        myInfo.status=STATUS_ONLINE;
        strcpy_s(myInfo.nick,30,"路明非");
        strcpy_s(myInfo.feeling,100,"something for nothing");
        RETURN_CLIENT_DATA(PROT_FRIEND_INFO,myInfo);

        {
            PROT_FRIEND_INFO friInfo;
            friInfo.userid=10;
            friInfo.userimgid=34;
            friInfo.status=STATUS_ONLINE;
            strcpy_s(friInfo.nick,30,"陈雯雯");
            strcpy_s(friInfo.feeling,100,"文学社社长");
            RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friInfo);
        }
        {
            PROT_FRIEND_INFO friInfo1;
            friInfo1.userid=11;
            friInfo1.userimgid=32;
            friInfo1.status=STATUS_NOONLINE;
            strcpy_s(friInfo1.nick,30,"老唐");
            strcpy_s(friInfo1.feeling,100,"星际争霸开黑伙伴");
            RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friInfo1);
        }
        {
            PROT_FRIEND_INFO friInfo2;
            friInfo2.userid=12;
            friInfo2.userimgid=29;
            friInfo2.status=STATUS_ONLINE;
            strcpy_s(friInfo2.nick,30,"诺诺");
            strcpy_s(friInfo2.feeling,100,"师姐");
            RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friInfo2);
        }
        {
            PROT_FRIEND_INFO friInfo3;
            friInfo3.userid=13;
            friInfo3.userimgid=11;
            friInfo3.status=STATUS_ONLINE;
            strcpy_s(friInfo3.nick,30,"芬格尔");
            strcpy_s(friInfo3.feeling,100,"废柴师兄");
            RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friInfo3);
        }

    }

}
inline void Get_ChatMessageToServer(PROT_CHAT_INFO_RQ r)
{
    qDebug()<<r.userid<<","<<r.message<<","<<r.friid;
    PROT_CHAT_INFO_RS s;
    s.userid=r.friid;
    s.friid=r.userid;
    if(r.friid==10)
    {
        s.result=RESPOND_SUCC;

        RETURN_CLIENT_DATA(PROT_CHAT_INFO_RS,s);

        //主动给路明非发送消息
        PROT_CHAT_INFO_RQ chatRq;
        chatRq.userid=10;
        chatRq.friid=r.userid;
        strcpy_s(chatRq.message,MESSAGE_MAX_LEN,"要加入文学社吗");
        RETURN_CLIENT_DATA(PROT_CHAT_INFO_RQ,chatRq);
    }
    if(r.friid==11)
    {
        s.result=RESPOND_FAIL;
        //不用给路明非发送消息
        RETURN_CLIENT_DATA(PROT_CHAT_INFO_RS,s);
    }
    if(r.friid==12)
    {
        s.result=RESPOND_SUCC;

        //主动给路明非发送消息

        PROT_CHAT_INFO_RQ chatRq;
         RETURN_CLIENT_DATA(PROT_CHAT_INFO_RS,s);
        chatRq.userid=12;
        chatRq.friid=r.userid;
        strcpy_s(chatRq.message,MESSAGE_MAX_LEN,"....");
        RETURN_CLIENT_DATA(PROT_CHAT_INFO_RQ,chatRq);

    }
    if(r.friid==13)
    {
        s.result=RESPOND_SUCC;
        //主动给路明非发送消息
        PROT_CHAT_INFO_RQ chatRq;
        RETURN_CLIENT_DATA(PROT_CHAT_INFO_RS,s);
        chatRq.userid=13;
        chatRq.friid=r.userid;
        strcpy_s(chatRq.message,MESSAGE_MAX_LEN,"带你去砸爆婚车的车轴");
        RETURN_CLIENT_DATA(PROT_CHAT_INFO_RQ,chatRq);
    }

}
//#define ADD_FRI_ACCEPT 0
//#define ADD_FRI_REJECT 1
//#define ADD_FRI_FRIOFF 2
//#define ADD_FRI_NOEXIST 3
inline void server_dealAddFriend(PROT_ADD_FRIEND_RQ Addfriend)
{
    qDebug()<<QString("%1向%2发送的好友请求").arg(Addfriend.usernick).arg(Addfriend.frinick);
    if(QString(Addfriend.frinick)=="绘梨衣") //同意
    {
        PROT_ADD_FRIEND_RS AddFriend;
        AddFriend.result=ADD_FRI_ACCEPT;
        AddFriend.friid=Addfriend.userid;
        strcpy_s(AddFriend.frinick,30,Addfriend.usernick);
        AddFriend.userid=20;
        strcpy_s(AddFriend.usernick,30,"绘梨衣");
        RETURN_CLIENT_DATA(PROT_ADD_FRIEND_RS,AddFriend);
        //绘梨衣客户端接受路明非客户端信息
        PROT_FRIEND_INFO myinfo;
        myinfo.userid=Addfriend.userid;
        myinfo.userimgid=5;
        strcpy_s(myinfo.nick,30,Addfriend.usernick);
        myinfo.status=STATUS_ONLINE;
        strcpy_s(myinfo.feeling,100,"something for nothing");
        //将myinfo 发送到绘梨衣客户端

        //将绘梨衣信息发送给路明非
        PROT_FRIEND_INFO friinfo;
        friinfo.userid=20;
        friinfo.userimgid=24;
        strcpy_s(friinfo.nick,30,"绘梨衣");
        myinfo.status=STATUS_ONLINE;
        strcpy_s(friinfo.feeling,100,"Sakura最好了");
        RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friinfo);
    }
    if(QString(Addfriend.frinick)=="赫尔佐格") //拒绝
    {
        PROT_ADD_FRIEND_RS AddFriend;
        AddFriend.result=ADD_FRI_REJECT;
        AddFriend.friid=Addfriend.userid;
        strcpy_s(AddFriend.frinick,30,Addfriend.usernick);
        AddFriend.userid=21;
        strcpy_s(AddFriend.usernick,30,"赫尔佐格");
        RETURN_CLIENT_DATA(PROT_ADD_FRIEND_RS,AddFriend);
    }
    if(QString(Addfriend.frinick)=="凯撒") //用户不在线
    {
        PROT_ADD_FRIEND_RS AddFriend;
        AddFriend.result=ADD_FRI_FRIOFF;
        AddFriend.friid=Addfriend.userid;
        strcpy_s(AddFriend.frinick,30,Addfriend.usernick);
        AddFriend.userid=22;
        strcpy_s(AddFriend.usernick,30,"凯撒");
        RETURN_CLIENT_DATA(PROT_ADD_FRIEND_RS,AddFriend);
    }
    if(QString(Addfriend.frinick)=="楚子航") //不存在
    {
        PROT_ADD_FRIEND_RS AddFriend;
        AddFriend.result=ADD_FRI_NOEXIST;
        AddFriend.friid=Addfriend.userid;
        strcpy_s(AddFriend.frinick,30,Addfriend.usernick);
        AddFriend.userid=-1;
        strcpy_s(AddFriend.usernick,30,"楚子航");
        RETURN_CLIENT_DATA(PROT_ADD_FRIEND_RS,AddFriend);
    }
}
inline void server_Deal_Friend_Rq(PROT_ADD_FRIEND_RS addFriRs)
{
    qDebug()<<"server_Deal_Friend_Rq";
    if(addFriRs.result==ADD_FRI_ACCEPT)
    {
        qDebug()<<"同意成为好友,通知到王婵的客户端";
        PROT_FRIEND_INFO myinfo;
        myinfo.userid=addFriRs.userid;
        myinfo.userimgid=5;
        strcpy_s(myinfo.nick,30,addFriRs.usernick);
        myinfo.status=STATUS_ONLINE;
        strcpy_s(myinfo.feeling,100,"something for nothing");
        //将此信息传入到源稚生的客户端
        //源稚生的信息
        PROT_FRIEND_INFO friinfo;
        friinfo.userid=25;
        friinfo.userimgid=30;
        strcpy_s(friinfo.nick,30,addFriRs.frinick);
        friinfo.status=STATUS_ONLINE;
        strcpy_s(friinfo.feeling,100,"【皇级混血种】");
        RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friinfo);
    }
    else if(addFriRs.result==ADD_FRI_REJECT)
    {
        qDebug()<<"对方拒绝添加好友";
    }
}
inline void server_dealUserOffline(PROT_USERLINEOFF)
{
    //通知所有好友你下线
}


#endif
