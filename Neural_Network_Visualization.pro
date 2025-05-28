QT       += core gui
QT += widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    backend.cpp \
    codegenerator.cpp \
    connectionitem.cpp \
    json_utils.cpp \
    layerblockitem.cpp \
    layeritem.cpp \
    main.cpp \
    mainwindow.cpp \
    networkvisualizer.cpp \
    neuronitem.cpp \
    programfragmentprocessor.cpp

HEADERS += \
    backend.h \
    codegenerator.h \
    connectionitem.h \
    json_utils.h \
    layerblockitem.h \
    layeritem.h \
    mainwindow.h \
    networkvisualizer.h \
    neuronitem.h \
    programfragmentprocessor.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
