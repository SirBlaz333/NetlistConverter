QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/circuit.cpp \
    src/element.cpp \
    src/elementio.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/netlistparser.cpp \
    src/signal.cpp

HEADERS += \
    headers/circuit.h \
    headers/element.h \
    headers/elementio.h \
    headers/mainwindow.h \
    headers/netlistparser.h \
    headers/signal.h

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md

RESOURCES += \
    resources/resources.qrc
