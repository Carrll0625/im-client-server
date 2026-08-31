 #include "mainwidget.h"
#include "ui_mainwidget.h"
#include"server/server.h"
#include"frienditem.h"
#include<QMessageBox>
#include<QInputDialog>
MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    m_pAddfri=m_menu.addAction("添加好友");
    m_psetting=m_menu.addAction("系统设置");
    ui->setupUi(this);
    QObject::connect(ui->pb_menu,SIGNAL(clicked()),this,SLOT(slots_menu()));
    QObject::connect(&m_menu,SIGNAL(triggered(QAction*)),this,SLOT(slots_clicked_Addfri(QAction*)));
}

MainWidget::~MainWidget()
{
    delete ui;
    for(pair<int,FriendItem*> pr:FriendMap)
    {
        if(pr.second)
        {
            delete  pr.second;
        }
    }
}

void MainWidget::setMyinfo(int imgid,QString nick,QString feeling)
{
    QIcon icon(QString(":/tx/%1.png").arg(imgid));
    ui->pb_img->setIcon(icon);
    //设置昵称和签名
    ui->label->setText(nick);
    ui->lineEdit->setText(feeling);
    //储存自己信息
    m_userimgid=imgid;
    m_nick=nick;
    m_feeling=feeling;


}
void MainWidget::setFriendInfo(int friid,int imgid,int status,QString nick,QString feeling)
{
    if(FriendMap.count(friid))
    {
        FriendItem* pfriItem=FriendMap[friid];
        pfriItem->setFriend(friid);
        pfriItem->showFriendInfo(imgid,status,nick,feeling);
    }
    else{       //新的好友
        FriendItem* pfriItem =new FriendItem;
        pfriItem->setFriend(friid);
        pfriItem->showFriendInfo(imgid,status,nick,feeling);
        QListWidgetItem* pItem=new QListWidgetItem;
        ui->lw_friendlist->addItem(pItem);
        ui->lw_friendlist->setItemWidget(pItem,pfriItem);

        FriendMap[friid]=pfriItem;
        //调整 QListWidgetItem 大小
        QSize s =pfriItem->size();
        pItem->setSizeHint(s);

    }

}
void  MainWidget::SetMessage(int friid,QString message)
{
    qDebug()<<"SetMessage";
    if(FriendMap.count(friid)>0)
    {
        FriendMap[friid]->setMessage(message);
    }
}
void MainWidget::slots_menu()
{
    qDebug()<<"slots_menu";
    //获取基于全局的坐标系，鼠标的位置
    QPoint po=QCursor::pos();
    QSize size=m_menu.sizeHint();
    po.setY(po.y()-size.height());
    m_menu.exec(po);
}
void MainWidget::slots_clicked_Addfri(QAction* action)
{
    qDebug()<<"slots_clicked_Addfri";
    if(action==m_pAddfri)
    {
        bool ok=false;
        QString friNick =QInputDialog::getText(this,"添加好友","请输入好友昵称",QLineEdit::Normal,"不为空的好友昵称",&ok).trimmed();
        if(ok)
        {
            if(friNick.isEmpty())
            {
                QMessageBox::information(this,"提示","好友昵称不能为空");
                return;
            }
        }

        for(const pair<int,FriendItem*>& px:FriendMap)
        {
            if(px.second->getNick()==friNick)
            {
                QMessageBox::information(this,"提示",friNick+"已经是你的好友了");
                return;
            }
            if(m_nick==friNick)
            {
                QMessageBox::information(this,"提示","不能添加自己为好友");
                return;
            }
            emit signals_addFriend(friNick);
            return;
        }
    }
    else if(action==m_psetting)
    {
        QMessageBox::information(this,"提示","功能暂未实现");
    }
}
void MainWidget::SetUserLineOff(int friid)
{
    if(FriendMap.count(friid))//检测是否是好友
    {
        FriendMap[friid]->setOffline();
    }
}
void MainWidget::closeEvent(QCloseEvent* )
{
    qDebug()<<"closeEvent";
    emit signals_closeEvent();
}
