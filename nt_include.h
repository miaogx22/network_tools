/*
 * Copyright (C) miaogx@aliyun.com
 */

#ifndef NT_INCLUDE_H
#define NT_INCLUDE_H

#include <QMainWindow>
#include <QLabel>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QTextBrowser>
#include <QDialog>

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <arpa/inet.h>

#include <QHash>
#include <QList>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QIODevice>
#include <QThread>
#include <QTimer>

#include <QDebug>

#define PROTOCOL_TYPE_UNKNOW     0
#define PROTOCOL_TYPE_TCP        1
#define PROTOCOL_TYPE_UDP        2
#define PROTOCOL_TYPE_BROADCAST  3
#define PROTOCOL_TYPE_MULTICAST  4

#define NT_TILE_STR             "network tools(网络调试工具)"
#define NT_VERSION_STR          "v1.0.1"

#define SEND_DATA_DEFAULT       "https://github.com/miaogx22/network_tools.git"

#ifdef Q_OS_WIN

#endif

#ifdef Q_OS_LINUX

#endif

#ifdef Q_OS_MAC

#endif

#endif // NT_INCLUDE_H
