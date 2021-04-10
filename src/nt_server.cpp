/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_server.h"

nt_server::nt_server()
{
    client_socket_list = new QList <nt_socket *>;

    con_count = 0;
}

nt_server::~nt_server()
{   
    delete_client_socket();
}

void nt_server::delete_client_socket()
{
    //遍历socket链表，从此表中移除
    //注意，删除之后并不需要再这里删除客户端对象,关闭时会触发对象的删除操作
    QList<nt_socket *>::iterator in;

    for(in=client_socket_list->begin(); in!=client_socket_list->end(); ++in) {
        nt_socket *t_socket = *in;
        client_socket_list->removeOne(t_socket);

        //由服务端创建的client_socket需要由server来关闭
        t_socket->tcp_socket->close();
    }
}

int nt_server::create_tcp_listen(QString ip, quint16 port)
{
     tcp_server = new QTcpServer;

    if(!tcp_server->listen(QHostAddress::AnyIPv4, port)){
        QString msg = tcp_server->errorString();
        emit nt_listen_failed(msg);
        tcp_server->close();
        return -1;
    }else {
        QString success = "listening " + ip + ":" + QString::number(port) + "successful";
        emit nt_listen_successful(success);
    }

    connect(tcp_server,SIGNAL(newConnection()),this,SLOT(accept_connection()));

    return 0;
}

int nt_server::create_udp_listen(QString ip, quint16 port)
{

    udp_server = new QUdpSocket(this);

    if(!udp_server->bind(QHostAddress::AnyIPv4, port)){
        QString msg = udp_server->errorString();
        emit nt_listen_failed(msg);
        return -1;
    }

    QString success = "listening udp" + ip + ":" + QString::number(port) + "successful";
    emit nt_listen_successful(success);

    connect(udp_server, SIGNAL(readyRead()), this, SLOT(udp_server_read()));

    return 0;
}

int nt_server::create_multicast_join(QString ip, quint16 port)
{
    udp_server = new QUdpSocket(this);

    if(!udp_server->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)){
        QString msg = udp_server->errorString();
        emit nt_listen_failed(msg);
        return -1;
    }

    bool result = udp_server->joinMulticastGroup(QHostAddress(ip));

    if(!result){
        QString msg = udp_server->errorString();
        emit nt_listen_failed(msg);
        return -1;
    }

    QString success = "join " + ip + ":" + QString::number(port) + "successful";
    emit nt_listen_successful(success);

    connect(udp_server, SIGNAL(readyRead()), this, SLOT(udp_server_read()));

    return 0;
}

void nt_server::accept_connection()
{
    nt_socket *client_socket = new nt_socket(nullptr, 0, nullptr, 0, PROTOCOL_TYPE_TCP);

    connect(client_socket, SIGNAL(nt_disconnected(QString)), this, SLOT(emit_nt_disconnected(QString)));
    connect(client_socket, SIGNAL(nt_recv(nt_session *)), this, SLOT(emit_nt_recv(nt_session *)));
    connect(client_socket, SIGNAL(nt_recv_count(nt_session *)), this, SLOT(emit_nt_recv_count(nt_session *)));
    connect(client_socket, SIGNAL(nt_send_count(nt_session *)), this, SLOT(emit_nt_send_count(nt_session *)));

    QTcpSocket *tcp_socket = tcp_server->nextPendingConnection();
    client_socket->tcp_client_assignment(tcp_socket);

    QString key = client_socket->tcp_socket->peerAddress().toString() + ":" + QString::number(client_socket->tcp_socket->peerPort());

    client_socket->current_session->session_key = key;

    //插入结构
    client_socket_list->append(client_socket);

    //通知界面有一个新的连接
    client_socket->current_session->status_msg = "new tcp connected from:" + key;
    client_socket->current_session->socket = client_socket;

    con_count += 1;
    emit nt_con_count(con_count);
    emit nt_new_connected(client_socket->current_session);

}


void nt_server::close_tcp_listen()
{
    tcp_server->close();
}

void nt_server::close_udp_listen()
{
    udp_server->close();
}


void nt_server::udp_server_read()
{
    QByteArray data;
    QHostAddress sender;
    quint16 senderPort = 0;
    int old_flag = 0;
    nt_socket *client_socket = nullptr;

    //循环读取数据
    while (udp_server->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udp_server->pendingDatagramSize());

        udp_server->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        data = QByteArray(datagram);
    }

    //获取此次接收的对端ip和端口, 构成key
    QString client_ip = convert_to_ipv4_addr(sender);

    QString key = client_ip + ":" + QString::number(senderPort);

    //遍历链表，找出此udp是否已经存在
    QList<nt_socket *>::iterator in;

    for(in=client_socket_list->begin(); in!=client_socket_list->end(); ++in) {
        nt_socket *t_socket = *in;
        QString tmp_key = t_socket->current_session->session_key;

        if(tmp_key == key){
            //说明是个老连接了
            client_socket = t_socket;
            old_flag = 1;
            break;
        }
    }

    //如果是新连接，需要添加结构,
    if(old_flag == 0){
        client_socket = new nt_socket(client_ip, senderPort, nullptr, 0, PROTOCOL_TYPE_UDP);
        client_socket->current_session->session_key = key;

        //插入结构
        client_socket_list->append(client_socket);

        //通知界面有一个新的连接
        client_socket->current_session->status_msg = "new udp client from:" + key;
        client_socket->current_session->socket = client_socket;
        client_socket->udp_socket = udp_server;                   //对于UDP连接的服务端,所有的session都只有一个udp_server.

        con_count += 1;
        emit nt_con_count(con_count);
        emit nt_new_connected(client_socket->current_session);

        connect(client_socket, SIGNAL(nt_send_count(nt_session *)), this, SLOT(emit_nt_send_count(nt_session *)));
    }

    //udp服务端并不是每个连接维系自己的收发，
    //而是在这里统一维系一个收发
    client_socket->current_session->recv_data = data;

    emit nt_server_recv(client_socket->current_session);

    if(data.size() > 0){
        client_socket->current_session->recv_count += data.size();
        emit nt_recv_count(client_socket->current_session);
    }

}



void  nt_server::server_error(QAbstractSocket::SocketError)
{
    QString msg = tcp_server->errorString();

    emit nt_listen_failed(msg);
}

/*
 * 不管是对端端口还是本端端口, 都会触发这个函数
 */
void  nt_server::emit_nt_disconnected(QString session_key)
{
    emit nt_disconnected(session_key);
    con_count -= 1;

    emit nt_con_count(con_count);

    //获取信号发送者
    nt_socket *tcp_socket =  qobject_cast<nt_socket *>(sender());
    delete tcp_socket;
}

void  nt_server::emit_nt_recv(nt_session *session)
{
    emit nt_server_recv(session);
}


void  nt_server::emit_nt_recv_count(nt_session *session)
{
    emit nt_recv_count(session);
}


void  nt_server::emit_nt_send_count(nt_session *session)
{
    emit nt_send_count(session);
}

void nt_server::count_clear(nt_socket *tcp_socket)
{
    tcp_socket->count_clear();

}

