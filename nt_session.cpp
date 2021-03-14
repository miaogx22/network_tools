/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_session.h"

nt_session::nt_session(QString pip, quint16 pport, QString lip, quint16 lport, int type)
{
    peer_ip_addr = pip;
    peer_port = pport;

    local_ip_addr = lip;
    local_port = lport;

    protocol_type = type;

    recv_count = 0;
    send_count = 0;

    recv_ishex = false;
    send_ishex = false;

    socket = nullptr;
}

nt_session::~nt_session()
{
}

void nt_session::session_reset(QString pip, quint16 pport, QString lip, quint16 lport, int type)
{
    if(pip != nullptr){
        peer_ip_addr = pip;
    }

    if(pport != 0){
        peer_port = pport;
    }

    if(lip != nullptr){
        local_ip_addr = lip;
    }

    if(lport != 0){
        local_port = lport;
    }

    if(type != PROTOCOL_TYPE_UNKNOW){
        protocol_type = type;
    }
}

int is_valid_ip(const char *ip)
{
    if(INADDR_NONE == inet_addr(ip)){
        return -1;
    }

    return 0;
}

QString convert_to_ipv4_addr(QHostAddress &addr)
{
    quint32  addr_origin = addr.toIPv4Address();
    QHostAddress addr_host = QHostAddress(addr_origin);
    QString  addr_str = addr_host.toString();
    return addr_str;
}
