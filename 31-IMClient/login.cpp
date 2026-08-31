#include "login.h"
#include "ui_login.h"
#include<QDebug>
#include<QMessageBox>

login::login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    QObject::connect(ui->pb_reg_login,SIGNAL(clicked()),this,SLOT(slots_register()));
    QObject::connect(ui->pb_reg_clear,SIGNAL(clicked()),this,SLOT(slots_reg_clear()));
    QObject::connect(ui->pb_login,SIGNAL(clicked()),this,SLOT(slots_log_login()));
    QObject::connect(ui->pb_clear,SIGNAL(clicked()),this,SLOT(slots_log_clear()));
}

login::~login()
{
    delete ui;
}

//注册里的登录按钮

void login::slots_register()
{
    qDebug()<<"注册中......";

    //获取昵称
    QString nick=ui->le_reg_nickname->text();
    if(nick.isEmpty())
    {
        QMessageBox::information(this,"提示","昵称不能为空");
        qDebug()<<"注册失败";
        return;
    }

    QString tel=ui->le_reg_number->text();
    if(tel.isEmpty())
    {
        QMessageBox::information(this,"提示","电话不能为空");
        qDebug()<<"注册失败";
        return;
    }
    else if(tel.size()!=11)
    {
        QMessageBox::information(this,"提示","电话号码长度不对");
        qDebug()<<"注册失败";
        return;
    }
    for(QChar c:tel)
    {
        if(c<'0'||c>'9')
        {
            QMessageBox::information(this,"提示","电话只能为数字");
            qDebug()<<"注册失败";
            return;
        }
    }

    QString password=ui->le_reg_password->text();
    QString passagain=ui->le_reg_OKpassword->text();

    if(password.size()<6)
    {
        QMessageBox::information(this,"提示","密码长度必须大于6位");
        qDebug()<<"注册失败";
        return;
    }
    else if(password!=passagain)
    {
         QMessageBox::information(this,"提示","两次输入的密码必须一致");
         qDebug()<<"注册失败";
         return;
    }

emit signals_LoginRegInfo(nick,tel,password);
}

//注册里的清空按钮
void login::slots_reg_clear()
{
    ui->le_reg_nickname->clear();
    ui->le_reg_password->clear();
    ui->le_reg_OKpassword->clear();
    ui->le_reg_number->clear();
}




void login::slots_log_login()
{
    QString tel=ui->le_log_number->text();
    if(tel.isEmpty())
    {
        QMessageBox::information(this,"提示","电话不能为空");
        return;
    }
    else if(tel.size()!=11)
    {
        QMessageBox::information(this,"提示","电话号码长度不对");
        return;
    }
    for(QChar c:tel)
    {
        if(c<'0'||c>'9')
        {
            QMessageBox::information(this,"提示","电话只能为数字");
            return;
        }
    }

    QString password=ui->le_log_password->text();

    if(password.size()<6)
    {
        QMessageBox::information(this,"提示","密码长度必须大于6位");
        return;
    }
    qDebug()<<"登录中......";
    emit signals_LoginInfo(tel,password);

}
void login::slots_log_clear(){
    ui->le_log_number->clear();
    ui->le_log_password->clear();
}
void login::closeEvent(QCloseEvent* )
{
   emit signals_closeLogin();
}







