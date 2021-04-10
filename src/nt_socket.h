/*
 * Copyright (C) miaogx@aliyun.com
 */

#ifndef NT_SOCKET_H
#define NT_SOCKET_H


#include "nt_include.h"
#include "nt_session.h"

#define IS_MY_FLAG        1
#define NO_MY_FLAG        0

class nt_socket: public QObject
{
    Q_OBJECT

public:
    nt_socket(QString pip, quint16 pport, QString lip, quint16 lport, int type);
    ~nt_socket();

    QTcpSocket *tcp_socket;

    int is_my_flag;               //标记此socket是自己创建的还是别人创建的

    nt_session *current_session;

    //tcp
    void tcp_client_assignment(QTcpSocket *socket);
    void tcp_close_socket();
    void tcp_client_connection();

    void tcp_clone_connection();
    qint64 tcp_client_send(QByteArray data);

    void count_clear();

    //udp
    QUdpSocket *udp_socket;

    void udp_client_create();
    void udp_broadcast_create();
    void udp_multicast_create();
    qint64 udp_client_send(QByteArray data);
    void udp_close_socket();

signals:
    void nt_recv(nt_session *session);

    void nt_connect_error(QString);
    void nt_join_error(QString);
    void nt_successful(nt_session *session);
    void nt_disconnected(QString session_key);

    void nt_recv_count(nt_session *session);
    void nt_send_count(nt_session *session);

    void nt_udp_successful(nt_session *session);

public slots:

    //tcp
    void tcp_connected_handle();
    void tcp_disconnected_handle();
    void tcp_read_data();

    void server_client_error(QAbstractSocket::SocketError);
    void udp_socket_error(QAbstractSocket::SocketError);
    //udp
    void udp_read_data();



};

#endif // NT_SOCKET_H
