#ifndef KERNEL_H
#define KERNEL_H
#include"login.h"
#include"../31-IMClient/net/def.h"
#include <QObject>
#include"mainwidget.h"
#include<QTimer>
#include"mediator/INetMediator.h"
class Kernel : public QObject
{
    Q_OBJECT
public:
    explicit Kernel(QObject *parent = nullptr);

signals:
    void signals_return_Client_Data(char* pbuf,int len,unsigned long ul);

public slots:
    void slots_Rec_Server_Data(char* pbuf,int len,unsigned long ul);
    void slots_Register(QString,QString,QString);
    void slots_Login(QString,QString);
    void slots_ReciveMsg(int,QString);
    void slots_FriendAdd(QString);
    void slots_updateInfoTimer();
    void slots_friOfflineTimer();
    void slots_addFriTimer();
    void slots_close();
public:
    void deal_Register_Rs(char* pbuf,int len,unsigned long ul);
    void deal_Login_Rs(char* pbuf,int len,unsigned long ul);
    void deal_friendInfo(char* pbuf,int len,unsigned long ul);
    void deal_MessageInfo_Rs(char* pbuf,int len,unsigned long ul);
    void deal_MessageInfo_Rq(char* pbuf,int len,unsigned long ul);
    void deal_AddFriend_Rs(char* pbuf,int len,unsigned long ul);
    void deal_UserLineOff(char* pbuf,int len,unsigned long ul);
    void deal_AddFriend_Rq(char* pbuf,int len,unsigned long ul);
    //gb2312转utf-8
    QString gb2312ToUtf8(char* src);
    //uft-8转gb2312
    void Utf8Togb2312(QString src,char* dst,int len);
public:
     using DealFunc=void (Kernel::*)   (char* ,  int   ,   unsigned long);
     DealFunc   m_protFuncArr[50];



    login *m_pLogin;
    MainWidget *m_pMainWidget;

    QTimer m_updateInfoTimer;
    QTimer m_friOfflineTimer;
    QTimer m_addFriTimer;
    INetMediator* m_pMediator;
};

#endif // KERNEL_H
