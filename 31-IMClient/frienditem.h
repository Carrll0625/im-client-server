#ifndef FRIENDITEM_H
#define FRIENDITEM_H
#include"chat.h"
#include <QWidget>

namespace Ui {
class FriendItem;
}

class FriendItem : public QWidget
{
    Q_OBJECT

public:
    explicit FriendItem(QWidget *parent = nullptr);
    ~FriendItem();
    void setFriend(int friid){friId=friid;}
    void showFriendInfo(int imgid,int status,QString nick,QString feeling);
    void setMessage(QString);
    QString getNick();
    void setOffline();
public slots:
    void slots_showChat();
    void slots_getChatMsg(QString);
    signals:
    void signals_sendKernelMsg(int,QString);
private:
    Ui::FriendItem *ui;
    int friId;
    int m_imgid;
    int m_status;
    QString m_nick;
    QString m_feeling;
    chat* pchat;
};

#endif // FRIENDITEM_H
