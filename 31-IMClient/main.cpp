#include "login.h"
#include"kernel.h"
#include <QApplication>

Kernel *pkernel;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //login w;
   // w.show();
    Kernel kernel;
    pkernel= &kernel;

    return a.exec();
}
