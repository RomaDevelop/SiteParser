QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    ads.cpp \
    html.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ads.h \
    html.h \
    mainwindow.h \
    tagnames.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += ../include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
