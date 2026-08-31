#include "frienditem.h"
#include "ui_frienditem.h"
#include"def/def.h"
#include<QBitmap>
#include<QDebug>
#include<kernel.h>
#include<QTime>
FriendItem::FriendItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendItem)
{
    extern Kernel* pkernel;
    pchat=new chat;
    ui->setupUi(this);
    QObject::connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slots_showChat()));
    QObject::connect(pchat,SIGNAL(signals_sendMsg(QString)),this,SLOT(slots_getChatMsg(QString)));
    QObject::connect(this,SIGNAL(signals_sendKernelMsg(int,QString)),pkernel,SLOT(slots_ReciveMsg(int,QString)));
}

FriendItem::~FriendItem()
{
    delete ui;
    if(pchat)
    {
        delete pchat;
        pchat=nullptr;
    }
}
void FriendItem::showFriendInfo(int imgid,int status,QString nick,QString feeling)
{
    if(status==STATUS_ONLINE)
    {
        QIcon icon(QString(":/tx/%1.png").arg(imgid));
        ui->pushButton->setIcon(icon);
    }else if(status==STATUS_NOONLINE)
    {
        QBitmap bit(QString(":/tx/%1.png").arg(imgid));
        ui->pushButton->setIcon(bit);
    }
    ui->label->setText(nick);
    ui->label_2->setText(feeling);

    //存储好友的信息
    m_imgid=imgid;
    m_status=status;
    m_nick=nick;
    m_feeling=feeling;
}
 void FriendItem::slots_showChat()
 {
     pchat->setWindowTitle(QString("与%1的聊天框").arg(m_nick));
     pchat->show();
 }
 void  FriendItem::slots_getChatMsg(QString msg)
 {
     qDebug()<<"slots_getChatMsg";
     emit signals_sendKernelMsg(friId,msg);
 }
 void FriendItem::setMessage(QString msg)
 {
    QString message=QString("%1[%2]:%3").arg(m_nick).arg(QTime::currentTime().toString()).arg(msg);
    pchat->setMessage(message);
 }
 QString FriendItem::getNick()
 {
     return m_nick;
 }
 void FriendItem::setOffline()
 {
     QBitmap bit(QString(":/tx/%1.png").arg(m_imgid));
     ui->pushButton->setIcon(bit);
 }
