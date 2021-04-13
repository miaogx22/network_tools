QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

win32 {
   RC_ICONS = nt_for_win.ico
}

unix {
    ICON = images/nt_for_mac.icns
}


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/nt_server.cpp \
    src/nt_session.cpp \
    src/nt_socket.cpp \
    src/nt_window.cpp \
    src/nt_window_file.cpp \
    src/nt_window_layout.cpp \
    src/nt_window_socket.cpp

HEADERS += \
    src/nt_include.h \
    src/nt_server.h \
    src/nt_session.h \
    src/nt_socket.h \
    src/nt_window.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

win32 {
    LIBS += -lWs2_32
}
