/*
 * Copyright (C) miaogx@aliyun.com
 */

#ifndef NT_SERVER_H
#define NT_SERVER_H

#include "nt_include.h"
#include "nt_socket.h"
#include "nt_session.h"

class nt_server: public QObject
{
    Q_OBJECT

public:
    nt_server();
    ~nt_server();

    QString local_ip_addr;
    quint16 local_port;

    int con_count;                                    //客户端连接的并发数

    QTcpServer *tcp_server;
    QUdpSocket *udp_server;                          //虚拟的udp_server。

    QList <nt_socket *> *client_socket_list;            //维系了本server下面所有的client结构

    int create_tcp_listen(QString ip, quint16 port);
    int create_udp_listen(QString ip, quint16 port);
    int create_multicast_listen(QString ip, quint16 port);
    void count_clear(nt_socket *tcp_socket);
    int delete_client_socket();

signals:
    void nt_server_recv(nt_session *session);
    void nt_listen_successful(QString msg);
    void nt_listen_failed(QString msg);
    void nt_new_connected(nt_session *session);

    void nt_disconnected(QString session_key);

    void nt_recv_count(nt_session *session);
    void nt_send_count(nt_session *session);

    void nt_con_count(int count);

public slots:
    void accept_connection();
    void udp_server_read();
    void server_error(QAbstractSocket::SocketError);

    void emit_nt_disconnected(QString session_key);
    void emit_nt_recv(nt_session *session);
    void emit_nt_recv_count(nt_session *session);
    void emit_nt_send_count(nt_session *session);


};

#endif // NT_SERVER_H
