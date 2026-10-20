#--------------------------------------------------------------------------------------------------
#--------------------------------------------------------------------------------------------------
#if(!equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 6))|greaterThan(QT_MAJOR_VERSION, 5) {
#  error( "Unsupported version ETH" )
#}
#--------------------------------------------------------------------------------------------------
CONFIG(debug, debug|release): DEFINES_NEW = DEBUG
DEFINES_NEW += QT_DEPRECATED_WARNINGS SUBJECT_PROJECT
for(DEF, DEFINES_NEW): {
  !contains(DEFINES, $$DEF) {
    DEFINES += $$DEF
  }
}
DEFINES += \
    VER_ETH="1.0"
#--------------------------------------------------------------------------------------------------
INCLUDEPATH += $$PWD
#--------------------------------------------------------------------------------------------------
SOURCES += \
    $$PWD/eth/ethcontroller.cpp \
    $$PWD/eth/corsairclient.cpp \
    $$PWD/eth/corsairserver.cpp \
#----- Socket
    $$PWD/eth/socket/socket.cpp \
    $$PWD/eth/socket/socketaddress.cpp \
    $$PWD/eth/socket/udpsocket.cpp \
    $$PWD/eth/socket/tcpsocket.cpp \
    $$PWD/eth/socket/udpserver.cpp \
    $$PWD/eth/socket/tcpserver.cpp \
    $$PWD/eth/socket/udpclient.cpp \
    $$PWD/eth/socket/tcpclient.cpp
#--------------------------------------------------------------------------------------------------
HEADERS += \
    $$PWD/eth/ethcontroller.h \
    $$PWD/eth/corsairclient.h \
    $$PWD/eth/corsairserver.h \
    $$PWD/eth/corsair_global.h \
#----- Socket
    $$PWD/eth/socket/socket.h \
    $$PWD/eth/socket/socketaddress.h \
    $$PWD/eth/socket/udpsocket.h \
    $$PWD/eth/socket/tcpsocket.h \
    $$PWD/eth/socket/udpserver.h \
    $$PWD/eth/socket/tcpserver.h \
    $$PWD/eth/socket/udpclient.h \
    $$PWD/eth/socket/tcpclient.h

#---------------------------------------------------------------------------
#----- 
#---------------------------------------------------------------------------
#DEFINES += PCAP
#CONFIG += pcap
pcap {
SOURCES += \
    $$PWD/eth/socket/pcapsocket.cpp \
    $$PWD/eth/socket/pcapclient.cpp \
    $$PWD/eth/socket/pcapserver.cpp
HEADERS += \
    $$PWD/eth/socket/pcapsocket.h \
    $$PWD/eth/socket/pcapclient.h \
    $$PWD/eth/socket/pcapserver.h
win32 {
    INCLUDEPATH += $${PWD}/PCAP/include
    LIST_NEW += -L$${PWD}/PCAP/lib/$${PLATFORM} -lwpcap
}
unix {
    LIST_NEW += -lpcap
}
}
#---------------------------------------------------------------------------
#----- 
#---------------------------------------------------------------------------
win32 {
LIST_NEW += -lws2_32
}

for(LIB, LIST_NEW): {
  !contains(LIBS, $$LIB) {
    LIBS += $$LIB
  }
}
#---------------------------------------------------------------------------
#----- 
#---------------------------------------------------------------------------
