#ifndef CHAT_H
#define CHAT_H

#include <QWidget>

namespace Ui {
class chat;
}

class chat : public QWidget
{
    Q_OBJECT

public:
    explicit chat(QWidget *parent = nullptr);
    ~chat();
public slots:
    //发送消息
    void slots_Sendmsg();
    //清空消息
    void slots_Msgclear();
    void setMessage(QString);
signals:
    void signals_sendMsg(QString);
private:
    Ui::chat *ui;
};

#endif // CHAT_H
