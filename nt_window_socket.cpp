/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_window.h"


void main_window::create_socket_client(QString ip, quint16 port, int type)
{
    socket_client = new nt_socket(ip, port, nullptr, 0, type);

    connect(socket_client, SIGNAL(nt_udp_successful(nt_session *)), this, SLOT(update_status_connected(nt_session *)));
    connect(socket_client, SIGNAL(nt_successful(nt_session *)), this, SLOT(update_status_connected(nt_session *)));
    connect(socket_client, SIGNAL(nt_disconnected(QString)), this, SLOT(update_status_disconnected(QString)));
    connect(socket_client, SIGNAL(nt_recv(nt_session *)), this, SLOT(show_data_handle(nt_session *)));

    connect(socket_client, SIGNAL(nt_recv_count(nt_session *)), this, SLOT(update_count_recv(nt_session *)));
    connect(socket_client, SIGNAL(nt_send_count(nt_session *)), this, SLOT(update_count_send(nt_session *)));

    if(type == PROTOCOL_TYPE_TCP){
        socket_client->tcp_client_connection();
    }

    if(type == PROTOCOL_TYPE_UDP){
        socket_client->udp_client_create();
    }

    if(type == PROTOCOL_TYPE_BROADCAST){
        socket_client->udp_broadcast_create();
    }
}


int main_window::create_tcp_listen(QString ip, quint16 port)
{
    socket_server = new nt_server();

    connect(socket_server, SIGNAL(nt_listen_successful(QString)), this, SLOT(listen_successful(QString)));         //监听成功
    connect(socket_server, SIGNAL(nt_listen_failed(QString)), this, SLOT(listen_failed(QString)));                 //新的连接
    connect(socket_server, SIGNAL(nt_new_connected(nt_session *)), this, SLOT(new_connected(nt_session *)));       //监听失败
    connect(socket_server, SIGNAL(nt_disconnected(QString)), this, SLOT(update_status_disconnected(QString)));
    connect(socket_server, SIGNAL(nt_server_recv(nt_session *)), this, SLOT(show_data_handle(nt_session *)));
    connect(socket_server, SIGNAL(nt_recv_count(nt_session *)), this, SLOT(update_count_recv(nt_session *)));
    connect(socket_server, SIGNAL(nt_send_count(nt_session *)), this, SLOT(update_count_send(nt_session *)));

    connect(socket_server, SIGNAL(nt_con_count(int)), this, SLOT(update_count_con(int)));

    return (socket_server->create_tcp_listen(ip, port));
}

void main_window::listen_successful(QString msg)
{
    statusBar()->showMessage(msg);
}

void main_window::listen_failed(QString msg)
{
    statusBar()->showMessage(msg);

    delete socket_server;

    start_button->setText("打开");
    start_button->repaint();
}


void main_window::new_connected(nt_session *session)
{
    //新会话插入hash表
    session_hash->insert(session->session_key, session);

    statusBar()->showMessage(session->status_msg);

    //更新源地址显示
    peer_box->addItem(session->session_key);
    peer_box->repaint();

}

/*
 * 注意：UDP无连接状态, 但是，在此项目组，虚拟了一个逻辑的udp连接
 */
int main_window::create_udp_listen(QString ip, quint16 port)
{
    socket_server = new nt_server();

    connect(socket_server, SIGNAL(nt_listen_successful(QString)), this, SLOT(listen_successful(QString)));         //监听成功
    connect(socket_server, SIGNAL(nt_listen_failed(QString)), this, SLOT(listen_failed(QString)));                 //新的连接
    connect(socket_server, SIGNAL(nt_new_connected(nt_session *)), this, SLOT(new_connected(nt_session *)));
    connect(socket_server, SIGNAL(nt_server_recv(nt_session *)), this, SLOT(show_data_handle(nt_session *)));
    connect(socket_server, SIGNAL(nt_recv_count(nt_session *)), this, SLOT(update_count_recv(nt_session *)));
    connect(socket_server, SIGNAL(nt_send_count(nt_session *)), this, SLOT(update_count_send(nt_session *)));

    connect(socket_server, SIGNAL(nt_con_count(int)), this, SLOT(update_count_con(int)));

    return (socket_server->create_udp_listen(ip, port));
}

int main_window::create_multicast_listen(QString ip, quint16 port)
{
    socket_server = new nt_server();

    connect(socket_server, SIGNAL(nt_listen_successful(QString)), this, SLOT(listen_successful(QString)));         //监听成功
    connect(socket_server, SIGNAL(nt_listen_failed(QString)), this, SLOT(listen_failed(QString)));                 //新的连接
    connect(socket_server, SIGNAL(nt_new_connected(nt_session *)), this, SLOT(new_connected(nt_session *)));
    connect(socket_server, SIGNAL(nt_server_recv(nt_session *)), this, SLOT(show_data_handle(nt_session *)));
    connect(socket_server, SIGNAL(nt_recv_count(nt_session *)), this, SLOT(update_count_recv(nt_session *)));
    connect(socket_server, SIGNAL(nt_send_count(nt_session *)), this, SLOT(update_count_send(nt_session *)));

    connect(socket_server, SIGNAL(nt_con_count(int)), this, SLOT(update_count_con(int)));

    return (socket_server->create_multicast_listen(ip, port));
}
