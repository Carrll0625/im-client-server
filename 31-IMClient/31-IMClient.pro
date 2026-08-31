QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS+=-lWs2_32

SOURCES += \
    chat.cpp \
    frienditem.cpp \
    kernel.cpp \
    main.cpp \
    login.cpp \
    mainwidget.cpp \
    mediator/INetMediator.cpp \
    mediator/TcpClientMediator.cpp \
    mediator/TcpServerMediator.cpp \
    mediator/UdpMediator.cpp \
    net/TcpClient.cpp \
    net/TcpServer.cpp \
    net/Udp.cpp

HEADERS += \
    chat.h \
    frienditem.h \
    kernel.h \
    login.h \
    mainwidget.h \
    mediator/INetMediator.h \
    mediator/TcpClientMediator.h \
    mediator/TcpServerMediator.h \
    mediator/UdpMediator.h \
    net/INet.h \
    net/TcpClient.h \
    net/TcpServer.h \
    net/Udp.h \
    net/def.h \
    server/server.h

FORMS += \
    chat.ui \
    frienditem.ui \
    login.ui \
    mainwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc \
    res.qrc
