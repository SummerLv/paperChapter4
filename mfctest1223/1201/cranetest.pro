# -------------------------------------------------
# Project created by QtCreator 2013-02-28T10:29:45
# -------------------------------------------------
QT += core \
    gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
TARGET = cranetest
TEMPLATE = app

# SOURCES += main.cpp\
# dialog.cpp \
# data.cpp \
# farm.cpp \
# dataprocess.cpp \
# uartAPI.cpp \
# tabdialog.cpp \
# basicDataDialog.cpp \
# proZoneDataTab.cpp \
# limitdatatab.cpp \
# interferecranetab.cpp \
# kdDataTab.cpp \
# # syszuxpinyin.cpp \
# # syszuxim.cpp \
# voiceplay.cpp \
# sqldb.cpp \
# mathAPI.cpp \
# angleTable.cpp \
# serialSendData.cpp \
# dataEncapsulateSpliteAPI.cpp \
# encspl.cpp \
# udp_socket_sender.cpp
# HEADERS  += dialog.h \
# data.h \
# farm.h \
# dataprocess.h \
# parameter.h \
# uartAPI.h \
# tabdialog.h \
# basicDataDialog.h \
# proZoneDataTab.h \
# limitdatatab.h \
# interferecranetab.h \
# kdDataTab.h \
# # ui_softKeyboard.h \
# #  syszuxpinyin.h \
# # syszuxim.h \
# voiceplay.h \
# sqldb.h \
# mathAPI.h \
# angleTable.h \
# serialSendData.h \
# dataEncapsulateSpliteAPI.h \
# encspl.h \
# udp_socket_sender.h
# LIBS += -lsyszuxpinyin
FORMS += syszuxpinyin.ui \
    softKeyboard.ui
CONFIG += thread \
    console
OTHER_FILES += 
HEADERS += voiceplay.h \
    udp_socket_sender.h \
    uartAPI.h \
    tabdialog.h \
    sqldb.h \
    sintab.h \
    serialSendData.h \
    proZoneDataTab.h \
    parameter.h \
    mathAPI.h \
    limitdatatab.h \
    kdDataTab.h \
    interferecranetab.h \
    gprs.h \
    finger.h \
    farm.h \
    encspl.h \
    dialog.h \
    dataprocess.h \
    dataEncapsulateSpliteAPI.h \
    data.h \
    basicDataDialog.h \
    angleTable.h \
    zigbeereceive.h \
    ui_syszuxpinyin.h \
    ui_softKeyboard.h \
    syszuxpinyin.h \
    syszuxim.h \
    temtab.h \
    videodev2_samsung.h \
    camtab.h \
    camtest.h \
    datagather.h \
    mfc_interface.h \
    SsbSipMfcApi.h \
    record.h \
    gpio.h \
    zigbeecfg.h \
    zigbeescreencfg.h \
    x264.h \
    x264_config.h
SOURCES += voiceplay.cpp \
    udp_socket_sender.cpp \
    uartAPI.cpp \
    tabdialog.cpp \
    sqldb.cpp \
    serialSendData.cpp \
    proZoneDataTab.cpp \
    mathAPI.cpp \
    main.cpp \
    limitdatatab.cpp \
    kdDataTab.cpp \
    interferecranetab.cpp \
    gprs.cpp \
    finger.cpp \
    farm.cpp \
    encspl.cpp \
    dialog.cpp \
    dataprocess.cpp \
    dataEncapsulateSpliteAPI.cpp \
    data.cpp \
    basicDataDialog.cpp \
    angleTable.cpp \
    zigbeereceive.cpp \
    syszuxpinyin.cpp \
    syszuxim.cpp \
    temtab.cpp \
    camtest.cpp \
    camtab.cpp \
    datagather.cpp \
    SsbSipMfcDecAPI.c \
    SsbSipMfcEncAPI.c \
    record.cpp \
    gpio.cpp \
    zigbeecfg.cpp \
    zigbeescreencfg.cpp
LIBS += -lasound
#LIBS += -lx264 \
#     -L/root/shiyanwendang/x264/cross/lib
LIBS += -lsyszuxpinyin \
    -L/usr/local/Trolltech/QtEmbedded-4.7.0-arm/lib/
#INCLUDEPATH += /usr/local/Trolltech/QtEmbedded-4.7.0-arm/lib/opencv/include \
#    /usr/local/Trolltech/QtEmbedded-4.7.0-arm/lib/opencv/include/opencv \
#    /usr/local/Trolltech/QtEmbedded-4.7.0-arm/lib/opencv/include/opencv2

#INCLUDEPATH +=  /root/shiyanwendang/x264/cross/lib
