#include "chat.h"
#include "ui_chat.h"
#include<QMessageBox>
#include<QTime>
chat::chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chat)
{
    ui->setupUi(this);
    QObject::connect(ui->pb_send,SIGNAL(clicked()),this,SLOT(slots_Sendmsg()));
    QObject::connect(ui->pb_clear,SIGNAL(clicked()),this,SLOT(slots_Msgclear()));
}

chat::~chat()
{
    delete ui;
}
void chat::slots_Sendmsg()
{
    QString chatMessage=ui->pte_send_message->toPlainText().trimmed();
    //检查文字内容是否为空
    if(chatMessage.isEmpty())
    {
        QMessageBox::information(this,"提示","输入消息不能为空");
        return;
    }
    //输出内容 使用append函数追加
    QString chat=QString("<font color='grey'>我[%1]:</font><p><font size='5'><font color='blue'>%2</font></p>").arg(QTime::currentTime().toString()).arg(chatMessage);
    ui->tb_chatbox->append(chat);

    ui->pte_send_message->setPlainText("");
    //将消息发送到服务端
    emit signals_sendMsg(chatMessage);
}
//清空消息
void chat::slots_Msgclear()
{

}
 void chat::setMessage(QString chat)
 {
      ui->tb_chatbox->append(chat);
 }
