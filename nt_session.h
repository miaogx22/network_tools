/*
 * Copyright (C) miaogx@aliyun.com
 */

#ifndef NT_SESSION_H
#define NT_SESSION_H

#include "nt_include.h"


class nt_socket;

/*
 * 这个结构标识一个会话
 */
class nt_session: public QObject
{
    Q_OBJECT

public:
    nt_session(QString pip, quint16 pport, QString lip, quint16 lport, int type);
    ~nt_session();

    QString peer_ip_addr;
    quint16 peer_port;

    QString local_ip_addr;
    quint16 local_port;

    int protocol_type;      //

    QString session_key;
    QString status_msg;

    QByteArray recv_data;
    QByteArray send_data;

    qint64 recv_count;
    qint64 send_count;

    bool recv_ishex;        //接收时是否以hex显示
    bool send_ishex;        //发送时是否以hex显示


    nt_socket *socket;        //指向的是每个session所属的那个nt_socket
    void session_reset(QString pip, quint16 pport, QString lip, quint16 lport, int type);

};

int is_valid_ip(const char *ip);
QString convert_to_ipv4_addr(QHostAddress &addr);

#endif // NT_SESSION_H
