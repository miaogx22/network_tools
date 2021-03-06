/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_socket.h"

nt_socket::nt_socket(QString pip, quint16 pport, QString lip, quint16 lport, int type)
{
    current_session = new nt_session(pip, pport, lip, lport, type);
    is_my_flag = NO_MY_FLAG;

}

nt_socket::~nt_socket()
{
}


/*
 * TCP socket --------------------------------------------------
 */
void nt_socket::tcp_client_connection()
{
    tcp_socket = new QTcpSocket;
    tcp_socket->connectToHost(current_session->peer_ip_addr, current_session->peer_port);

    is_my_flag = IS_MY_FLAG;

    connect(tcp_socket, SIGNAL(connected()), this, SLOT(tcp_connected_handle()));
    connect(tcp_socket, SIGNAL(disconnected()), this, SLOT(tcp_disconnected_handle()));
    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(tcp_read_data()));

    connect(tcp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(server_client_error(QAbstractSocket::SocketError)));
}

void nt_socket::tcp_close_socket()
{
    if(is_my_flag == IS_MY_FLAG){
        tcp_socket->close();
    }
}

void nt_socket::tcp_client_assignment(QTcpSocket *socket)
{
    tcp_socket = socket;

    connect(tcp_socket, SIGNAL(connected()), this, SLOT(tcp_connected_handle()));
    connect(tcp_socket, SIGNAL(disconnected()), this, SLOT(tcp_disconnected_handle()));
    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(tcp_read_data()));

    connect(tcp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(server_client_error(QAbstractSocket::SocketError)));
}

void nt_socket::tcp_connected_handle()
{
    current_session->status_msg = "connect successful.";

    //tcp client端将本地的ip和端口作为session_key值
    QString key = tcp_socket->localAddress().toString() + ":" + QString::number(tcp_socket->localPort());
    current_session->session_key = key;

    emit nt_successful(current_session);
}

void nt_socket::tcp_disconnected_handle()
{
    current_session->status_msg = "tcp session disconnected:" + current_session->session_key;

    emit nt_disconnected(current_session->session_key);
}

void nt_socket::tcp_read_data()
{
    current_session->recv_data = tcp_socket->readAll();

    emit nt_recv(current_session);
    current_session->recv_count += current_session->recv_data.size();

    emit nt_recv_count(current_session);

}

qint64 nt_socket::tcp_client_send(QByteArray data)
{
    qint64 ret = tcp_socket->write(data);

    if(ret > 0){
        current_session->send_count += ret;
        emit nt_send_count(current_session);
    }

    return ret;
}

void nt_socket::count_clear()
{
    current_session->recv_count = 0;
    current_session->send_count = 0;

    emit nt_recv_count(current_session);
    emit nt_send_count(current_session);
}

void nt_socket::server_client_error(QAbstractSocket::SocketError)
{
    QString msg = tcp_socket->errorString();
    emit nt_connect_error(msg);
}

/*
 * UDP socket --------------------------------------------------
 */
void nt_socket::udp_client_create()
{
    udp_socket = new QUdpSocket;

    is_my_flag = IS_MY_FLAG;

    //如下, 发送0字节数据，只是为了能够获取到本地端口
    if(current_session->protocol_type == PROTOCOL_TYPE_BROADCAST){
        udp_socket->writeDatagram(nullptr, 0, QHostAddress::Broadcast,current_session->peer_port);
    } else if(current_session->protocol_type == PROTOCOL_TYPE_MULTICAST){
        udp_socket->writeDatagram(nullptr, 0, QHostAddress(current_session->peer_ip_addr),current_session->peer_port);
    } else {
        udp_socket->writeDatagram(nullptr, 0, QHostAddress(current_session->peer_ip_addr), current_session->peer_port);
    }

    current_session->status_msg = "udp create successful.";

    //创建udp client时 将本地的ip和端口作为session_key值
    QString key = udp_socket->localAddress().toString() + ":" + QString::number(udp_socket->localPort());
    current_session->session_key = key;

    emit nt_udp_successful(current_session);

    connect(udp_socket, SIGNAL(readyRead()), this, SLOT(udp_read_data()));
    connect(udp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(udp_socket_error(QAbstractSocket::SocketError)));
}

void nt_socket::udp_broadcast_create()
{
    udp_socket = new QUdpSocket;

    is_my_flag = IS_MY_FLAG;

    //如下, 发送0字节数据，只是为了能够获取到本地端口
    udp_socket->writeDatagram(nullptr, 0, QHostAddress::Broadcast,current_session->peer_port);

    current_session->status_msg = "udp broadcast create successful.";

    //创建udp client时 将本地的ip和端口作为session_key值
    QString key = udp_socket->localAddress().toString() + ":" + QString::number(udp_socket->localPort());
    current_session->session_key = key;

    emit nt_udp_successful(current_session);

    connect(udp_socket, SIGNAL(readyRead()), this, SLOT(udp_read_data()));
    connect(udp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(udp_socket_error(QAbstractSocket::SocketError)));
}

void nt_socket::udp_multicast_create()
{
    udp_socket = new QUdpSocket;

    is_my_flag = IS_MY_FLAG;

    if(!udp_socket->bind(QHostAddress::AnyIPv4, current_session->peer_port, QUdpSocket::ShareAddress)){
        QString msg = udp_socket->errorString();
        emit nt_join_error(msg);
        return;
    }

    bool result = udp_socket->joinMulticastGroup(QHostAddress(current_session->peer_ip_addr));
    if(!result){
        QString msg = udp_socket->errorString();
        emit nt_join_error(msg);
        return;
    }

    //如下, 发送0字节数据，只是为了能够获取到本地端口
   udp_socket->writeDatagram(nullptr, 0, QHostAddress(current_session->peer_ip_addr),current_session->peer_port);

   current_session->status_msg = "udp multicast join successful.";

   //创建udp client时 将本地的ip和端口作为session_key值
   QString key = udp_socket->localAddress().toString() + ":" + QString::number(udp_socket->localPort());
   current_session->session_key = key;

    //设置接收缓冲区大小
    //udp_socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,1024*1024*8);

    emit nt_udp_successful(current_session);

    connect(udp_socket, SIGNAL(readyRead()), this, SLOT(udp_read_data()));
    connect(udp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(udp_socket_error(QAbstractSocket::SocketError)));
}


qint64 nt_socket::udp_client_send(QByteArray data)
{
    qint64 ret = 0;

    if(current_session->protocol_type == PROTOCOL_TYPE_BROADCAST){
        ret = udp_socket->writeDatagram(data, data.size(), QHostAddress::Broadcast,current_session->peer_port);
    } else if(current_session->protocol_type == PROTOCOL_TYPE_MULTICAST){
        ret = udp_socket->writeDatagram(data, data.size(), QHostAddress(current_session->peer_ip_addr),current_session->peer_port);
    } else {
        ret = udp_socket->writeDatagram(data, data.size(), QHostAddress(current_session->peer_ip_addr), current_session->peer_port);
    }


    if(ret > 0){
        current_session->send_count += ret;
        emit nt_send_count(current_session);
    }

    return ret;
}

void nt_socket::udp_read_data()
{
    QByteArray data;
    while (udp_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udp_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udp_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        data = QByteArray(datagram);
    }

    current_session->recv_data = data;

    emit nt_recv(current_session);

    if(data.size() > 0){
        current_session->recv_count += data.size();
        emit nt_recv_count(current_session);
    }


}

void nt_socket::udp_socket_error(QAbstractSocket::SocketError)
{
    QString msg = udp_socket->errorString();
    udp_socket->close();

    emit nt_connect_error(msg);
}

void nt_socket::udp_close_socket()
{
    if(is_my_flag == IS_MY_FLAG){
        udp_socket->close();
    }
}
