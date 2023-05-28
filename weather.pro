QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# 版本信息
VERSION = 1.0.0.1

# 产品名称
QMAKE_TARGET_PRODUCT = "Weather_CHINA"
TARGET = "Weather_CHINA"

# 文件说明
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on Qt 5.13.2"

# 版权信息
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2023 GaoYuan"

# 中文（简体）
RC_LANG = 0x0804

RC_FILE += app.rc

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES +=
