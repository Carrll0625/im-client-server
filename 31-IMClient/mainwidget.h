#ifndef MAINWIDGET_H
#define MAINWIDGET_H
#include"chat.h"
#include <QWidget>
#include <map>
#include<frienditem.h>
#include<QMenu>
using namespace std;
namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
public:
    void setUserId(int id){ m_userid=id;   }
    int getUserId(){return m_userid;   }

    void setMyinfo(int imgid,QString nick,QString feeling);
    void setFriendInfo(int friid,int imgid,int status,QString nick,QString feeling);
    void SetMessage(int friid,QString message);
    void Setusernick(QString usernick){m_nick=usernick;}
    QString Getusernick(){return m_nick;}
    void SetUserLineOff(int);
private:
    Ui::MainWidget *ui;

    int m_userid;   //存储当前登录用户的id
    int m_userimgid;
    int m_friid;
    QString m_nick;
    QString m_feeling;
    //存储好友列表信息的map对
    map<int,FriendItem*> FriendMap;
    QMenu m_menu;
    QAction* m_pAddfri;
    QAction* m_psetting;
public slots:
    void slots_menu();
    void slots_clicked_Addfri(QAction*);
 public:
    virtual void closeEvent(QCloseEvent* ) override;

signals:
    void signals_addFriend (QString);
    void signals_closeEvent();
};

#endif // MAINWIDGET_H
