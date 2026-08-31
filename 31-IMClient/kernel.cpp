#include "kernel.h"
#include<QDebug>
#include"../31-IMClient/server/server.h"
#include<QMessageBox>
#include"mainwidget.h"
#include"login.h"
#include"mediator/TcpClientMediator.h"
#include<QTextCodec>
Kernel::Kernel(QObject *parent) : QObject(parent),m_pLogin(new login),m_protFuncArr{nullptr},m_pMainWidget(new MainWidget)
{
    m_pLogin->show();
    QObject::connect(m_pLogin,&login::signals_LoginRegInfo,this,&Kernel::slots_Register);
    QObject::connect(m_pLogin,&login::signals_LoginInfo,this,&Kernel::slots_Login);
    QObject::connect(this,&Kernel::signals_return_Client_Data,this,&Kernel::slots_Rec_Server_Data);
    QObject::connect(m_pMainWidget,SIGNAL(signals_addFriend(QString)),this,SLOT(slots_FriendAdd(QString)));
    QObject::connect(m_pMainWidget,SIGNAL(signals_closeEvent()),this,SLOT(slots_close()));
    QObject::connect(m_pLogin,SIGNAL(signals_closeLogin()),this,SLOT(slots_close()));
    m_protFuncArr[DEF_PROT_DESIGNER_RS-DEF_PROT_BASE]=&Kernel::deal_Register_Rs;
    m_protFuncArr[DEF_PROT_LOGIN_RS-DEF_PROT_BASE]=&Kernel::deal_Login_Rs;
    m_protFuncArr[DEF_PROT_FRIENDINFO-DEF_PROT_BASE]=&Kernel::deal_friendInfo;
    m_protFuncArr[DEF_PROT_CHAT_INFO_RS-DEF_PROT_BASE]=&Kernel::deal_MessageInfo_Rs;
    m_protFuncArr[DEF_PROT_CHAT_INFO_RQ-DEF_PROT_BASE]=&Kernel::deal_MessageInfo_Rq;
    m_protFuncArr[DEF_PROT_ADD_FRIEND_RS-DEF_PROT_BASE]=&Kernel::deal_AddFriend_Rs;
    m_protFuncArr[DEF_PROT_USERLINEOFF-DEF_PROT_BASE]=&Kernel::deal_UserLineOff;
    m_protFuncArr[DEF_PROT_ADD_FRIEND_RQ-DEF_PROT_BASE]=&Kernel::deal_AddFriend_Rq;

    m_pMediator=new TcpClientMediator;
    connect(m_pMediator,&INetMediator::signals_Rec_Server_Data,this,&Kernel::slots_Rec_Server_Data);
    if( !m_pMediator->openNet())
    {
        QMessageBox::about(m_pLogin,"提示","打开网络失败");
        exit(1);
    }

}
void Kernel::slots_Rec_Server_Data(char* pbuf,int len,unsigned long ul)
{
    qDebug()<<"slots_Rec_Server_Data";

    protType pt=*(protType*)pbuf;
    if(pt>=DEF_PROT_BASE&&pt<(DEF_PROT_BASE+100))
    {
        //pt-DEF_PROT_BASE;
        DealFunc dealFunc =m_protFuncArr[pt-DEF_PROT_BASE];
        if(dealFunc)
        {
            (this->*dealFunc)(pbuf,len,ul);
        }

        else
        {
            QMessageBox::information(m_pLogin,"提示","处理函数为空");
        }
    }
    else
    {
        QMessageBox::information(m_pLogin,"提示","解析错误");
    }
    //回收空间
    if(pbuf)
    {
        delete []pbuf;
        pbuf=nullptr;
    }





   /* if(pt==DEF_PROT_DESIGNER_RS)
    {
            PROT_REGISTER_RS* pRegisterRs=(PROT_REGISTER_RS*)pbuf;
            if(pRegisterRs->result==REGISTER_SUCCESS)
            {
                QMessageBox::information(m_pLogin,"提示","注册成功");
            }
            else if(pRegisterRs->result==REGISTER_FAIL)
            {
                    QMessageBox::information(m_pLogin,"提示","注册失败");
            }
     }


    if(pt==DEF_PROT_LOGIN_RS)
    {
        PROT_LOGIN_RS* pLoginRs=(PROT_LOGIN_RS*)pbuf;
        if(pLoginRs->result==LOGIN_SUCCESS)
        {
            //隐藏登录框
            m_pLogin->hide();

            //显示主聊天窗口

            //存储登录用户id

        }
        else if(pLoginRs->result==LOGIN_NOEXIST)
        {
                QMessageBox::information(m_pLogin,"提示","用户不存在");
        }
        else if(pLoginRs->result==LOGIN_PASSERR)
        {
                QMessageBox::information(m_pLogin,"提示","密码错误");
        }
    }*/

}

void Kernel::slots_Register(QString nick,QString tel,QString password){

    PROT_REGISTER_RQ registerRq;

    //strcpy_s(registerRq.nick,30,nick.toStdString().c_str());
    Utf8Togb2312(nick,registerRq.nick,sizeof (registerRq));
    strcpy_s(registerRq.tel,15,tel.toStdString().c_str());
    strcpy_s(registerRq.password,20,password.toStdString().c_str());

    m_pMediator->sendData((char*)&registerRq,sizeof(registerRq),6);


}

void Kernel::slots_Login(QString tel ,QString password ){

    PROT_LOGIN_RQ loginRq;

    strcpy_s(loginRq.tel,15,tel.toStdString().c_str());
    strcpy_s(loginRq.password,20,password.toStdString().c_str());

    m_pMediator->sendData((char*)&loginRq,sizeof(loginRq),6);

}


void Kernel::deal_Register_Rs(char* pbuf,int len,unsigned long ul)
{
    PROT_REGISTER_RS* pRegisterRs=(PROT_REGISTER_RS*)pbuf;
    if(pRegisterRs->result==REGISTER_SUCCESS)
    {
        QMessageBox::information(m_pLogin,"提示","注册成功");
    }
    else if(pRegisterRs->result==REGISTER_NICK_EXISTS)
    {
            QMessageBox::information(m_pLogin,"提示","注册失败,昵称已被占用");
    }
    else if(pRegisterRs->result==REGISTER_TEL_EXISTS)
    {
            QMessageBox::information(m_pLogin,"提示","注册失败,电话号已被占用");
    }
}

void Kernel::deal_Login_Rs(char* pbuf,int len,unsigned long ul)
{
    PROT_LOGIN_RS* pLoginRs=(PROT_LOGIN_RS*)pbuf;
    if(pLoginRs->result==LOGIN_SUCCESS)
    {
        //隐藏登录框
        m_pLogin->hide();
        //显示主聊天窗口
        m_pMainWidget->show();

        //存储登录用户id
         m_pMainWidget->setUserId(pLoginRs->userID);

        qDebug()<<"获取用户id：" <<m_pMainWidget->getUserId();

    }
    else if(pLoginRs->result==LOGIN_NOEXIST)
    {
            QMessageBox::information(m_pLogin,"提示","用户不存在");
    }
    else if(pLoginRs->result==LOGIN_PASSERR)
    {
            QMessageBox::information(m_pLogin,"提示","密码错误");
    }
}
 void Kernel::deal_friendInfo(char* pbuf,int len,unsigned long ul)
 {
     qDebug()<<"deal_friendInfo";
     PROT_FRIEND_INFO* friinfo=(PROT_FRIEND_INFO*) pbuf;
     if(friinfo->userid==m_pMainWidget->getUserId())
     {
         QString nick=gb2312ToUtf8(friinfo->nick);
         QString feeling=gb2312ToUtf8(friinfo->feeling);
         m_pMainWidget->setMyinfo(friinfo->userimgid,nick,feeling);
     }
     else
     {
         QString nick=gb2312ToUtf8(friinfo->nick);
         QString feeling=gb2312ToUtf8(friinfo->feeling);
         m_pMainWidget->setFriendInfo(friinfo->userid,friinfo->userimgid,friinfo->status,nick,feeling);
     }
 }
 void Kernel::deal_MessageInfo_Rs(char* pbuf,int len,unsigned long ul)
 {
     qDebug()<<"deal_MessageInfo_Rs";
     PROT_CHAT_INFO_RS* message=(PROT_CHAT_INFO_RS*) pbuf;
     if(message->result==RESPOND_SUCC)
     {
         m_pMainWidget->SetMessage(message->userid,"已送达");
     }
     else if(message->result==RESPOND_FAIL)
     {
            //todo:
         m_pMainWidget->SetMessage(message->userid,"好友已下线");
     }
 }
 void  Kernel::deal_MessageInfo_Rq(char* pbuf,int len,unsigned long ul)
{
 qDebug()<<"deal_MessageInfo_Rq";
 PROT_CHAT_INFO_RQ* message=(PROT_CHAT_INFO_RQ*) pbuf;
    m_pMainWidget->SetMessage(message->userid,message->message);
 }
void Kernel::deal_AddFriend_Rs(char* pbuf,int len,unsigned long ul)
{
    qDebug()<<"deal_AddFriend_Rs";
    PROT_ADD_FRIEND_RS* addFriend=(PROT_ADD_FRIEND_RS*) pbuf;
    QString nick=gb2312ToUtf8(addFriend->usernick);
    if(addFriend->result==ADD_FRI_ACCEPT)
    {
        QMessageBox::information(m_pMainWidget,"提示",QString(addFriend->usernick)+"同意了你的好友请求");


    }
    else if(addFriend->result==ADD_FRI_REJECT)
    {
        QMessageBox::information(m_pMainWidget,"提示",QString(addFriend->usernick)+"拒绝了你的好友请求");
    }
    else if(addFriend->result==ADD_FRI_FRIOFF)
    {
        QMessageBox::information(m_pMainWidget,"提示",nick+"用户不在线");
    }
    else if(addFriend->result==ADD_FRI_NOEXIST)
    {
        QMessageBox::information(m_pMainWidget,"提示",nick+"此用户不存在");
    }

}
void Kernel::deal_UserLineOff(char* pbuf,int len,unsigned long ul)
{
    PROT_USERLINEOFF* friid=(PROT_USERLINEOFF*) pbuf;
    m_pMainWidget->SetUserLineOff(friid->userid);
}
void Kernel::deal_AddFriend_Rq(char* pbuf,int len,unsigned long ul)
{
    qDebug()<<"deal_AddFriend_Rq";
    PROT_ADD_FRIEND_RQ* paddFriRq=(PROT_ADD_FRIEND_RQ*)pbuf;
    QMessageBox::StandardButton but =QMessageBox::information(m_pMainWidget,"提示",QString(paddFriRq->usernick)+"想添加你为好友,是否同意？",QMessageBox::Yes|QMessageBox::No);
            PROT_ADD_FRIEND_RS addFriRs;
        addFriRs.userid=m_pMainWidget->getUserId();
        strcpy_s(addFriRs.usernick,30,m_pMainWidget->Getusernick().toStdString().c_str());
        qDebug()<<paddFriRq->userid;
        addFriRs.friid=paddFriRq->userid;
        strcpy_s(addFriRs.frinick,30,paddFriRq->usernick);
        if(but==QMessageBox::Yes)
        {
            addFriRs.result=ADD_FRI_ACCEPT;
        }
        else if(but==QMessageBox::No)
        {
            addFriRs.result=ADD_FRI_REJECT;
        }
       m_pMediator->sendData((char*)&addFriRs,sizeof (addFriRs),4);
}
 void Kernel::slots_ReciveMsg(int userid,QString msg)
{
    qDebug()<<"slots_ReciveMsg" <<userid<<","<<msg;
    PROT_CHAT_INFO_RQ r;
    r.userid=m_pMainWidget->getUserId();
    strcpy_s(r.message,18432,msg.toStdString().c_str());
    r.friid=userid;

    m_pMediator->sendData((char*)&r,sizeof(r),3);
}
void Kernel::slots_FriendAdd(QString frinick)
{
    PROT_ADD_FRIEND_RQ addFriend;
    addFriend.userid=m_pMainWidget->getUserId();
    //strcpy_s(addFriend.frinick,20,frinick.toStdString().c_str());
    Utf8Togb2312(frinick,addFriend.frinick,sizeof(PROT_ADD_FRIEND_RQ));
    strcpy_s(addFriend.usernick,20,m_pMainWidget->Getusernick().toStdString().c_str());

    m_pMediator->sendData((char*)&addFriend,sizeof(addFriend),3);
}
void Kernel::slots_updateInfoTimer()
{
    qDebug()<<"slots_updateInfoTimer";
    PROT_FRIEND_INFO friInfo1;
    friInfo1.userid=11;
    friInfo1.userimgid=32;
    friInfo1.status=STATUS_ONLINE;
    strcpy_s(friInfo1.nick,30,"诺顿");
    strcpy_s(friInfo1.feeling,100,"青铜与火之王");
    RETURN_CLIENT_DATA(PROT_FRIEND_INFO,friInfo1);
}
void Kernel::slots_friOfflineTimer()
{
    qDebug()<<"slots_friOfflineTimer";
    PROT_USERLINEOFF userLineoff;
    userLineoff.userid=11;
    RETURN_CLIENT_DATA(PROT_USERLINEOFF,userLineoff);
}
void Kernel::slots_addFriTimer()
{
    qDebug()<<"slots_addFriTimer";
    PROT_ADD_FRIEND_RQ addfri;
    strcpy_s(addfri.usernick,15,"源稚生");
    addfri.userid=25;
    RETURN_CLIENT_DATA(PROT_ADD_FRIEND_RQ,addfri);
}
void Kernel::slots_close()
{
    PROT_USERLINEOFF myOff;
    myOff.userid= m_pMainWidget->getUserId();
    m_pMediator->sendData((char*)&myOff,sizeof(myOff),3);
    //回收空间
    //回收登录框
    if(m_pLogin)
    {
        delete m_pLogin;
        m_pLogin=nullptr;
    }
    //回收聊天主题窗口
    if(m_pMainWidget)
    {
        delete  m_pMainWidget;
        m_pMainWidget=nullptr;
    }

}

//gb2312转utf-8
QString Kernel::gb2312ToUtf8(char* src)
{
    QTextCodec* dc=QTextCodec::codecForName("gb2312");
    return dc->toUnicode(src);
}
//uft-8转gb2312
void Kernel::Utf8Togb2312(QString src,char* dst,int len)
{
    QTextCodec* dc=QTextCodec::codecForName("gb2312");
    QByteArray ba= dc->fromUnicode(src);
    strcpy_s(dst,len,ba.data());
}











