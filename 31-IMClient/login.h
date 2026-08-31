#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class login; }
QT_END_NAMESPACE

class login : public QMainWindow
{
    Q_OBJECT

public:
    login(QWidget *parent = nullptr);
    ~login();
public slots:
    void slots_register();
    void slots_reg_clear();
    void slots_log_login();
    void slots_log_clear();

signals:
    void signals_LoginRegInfo(QString,  QString,  QString  );
    void signals_LoginInfo(QString,QString);
    void signals_closeLogin();



private:
    Ui::login *ui;
public:
   virtual void closeEvent(QCloseEvent* ) override;
};
#endif // LOGIN_H
