QT = core

CONFIG += c++17 cmdline



win32 {
####
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
####
    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/debug/lib/ -llibssl
        LIBS += -L$$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/debug/lib/ -llibcrypto
        LIBS += -L$$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/debug/lib/ -llibcurl-d
    }
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/lib/ -llibssl
        LIBS += -L$$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/lib/ -llibcrypto
        LIBS += -L$$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/lib/ -llibcurl
    }
####
    INCLUDEPATH += $$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/include
    DEPENDPATH += $$PWD/../vcpkg/win10/vcpkg_installed/x64-windows/include
####
    #程序版本
    VERSION = 1.0.1
    #公司名称
    QMAKE_TARGET_COMPANY ="vildoc.com"
    #程序说明
    QMAKE_TARGET_DESCRIPTION = "vildoc.com"
    #版权信息
    QMAKE_TARGET_COPYRIGHT = "vildoc.com"
    #程序名称
    QMAKE_TARGET_PRODUCT = "alisignv3"
}


unix {
####
    QMAKE_CXXFLAGS += -g -rdynamic
    QMAKE_RPATHDIR += ./
####
#    LIBS += -lgd
####
    LIBS += -L$$PWD/../vcpkg/ubuntu2204/vcpkg_installed/x64-linux/lib/ -lssl
    LIBS += -L$$PWD/../vcpkg/ubuntu2204/vcpkg_installed/x64-linux/lib/ -lcrypto
    LIBS += -L$$PWD/../vcpkg/ubuntu2204/vcpkg_installed/x64-linux/lib/ -lcurl
####
    INCLUDEPATH += $$PWD/../vcpkg/ubuntu2204/vcpkg_installed/x64-linux/include
    DEPENDPATH += $$PWD/../vcpkg/ubuntu2204/vcpkg_installed/x64-linux/include
}



SOURCES += \
        aliyunRequest.cpp \
        curlrequest.cpp \
        main.cpp \
        plugins/CJsonObject.cpp \
        plugins/cJSON.c

HEADERS += \
    aliyunRequest.h \
    curlrequest.h \
    plugins/CJsonObject.h \
    plugins/cJSON.h
