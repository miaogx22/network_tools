/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_window.h"

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
{
    type_index = TCP_CLIENT_INDEX;            //设置初始的类型选项为tcp客户端

    lf_flag = NT_FLAG_NO;
    recv_file_flag = NT_FLAG_NO;
    send_file_flag = NT_FLAG_NO;
    loop_send_flag = NT_FLAG_NO;
    send_stop_flag = NT_FLAG_NO;
    recv_hex_ascii_flag = NT_FLAG_ASCII;
    send_hex_ascii_flag = NT_FLAG_ASCII;

    loop_time = 0;

    session_hash = new  QHash<QString, nt_session*>;

    QString title = QString::fromUtf8(NT_TILE_STR) +  QString::fromUtf8(NT_VERSION_STR);
    setWindowTitle(title);
    this->resize( QSize(600,600));

    record_for_loop = nullptr;
    loop_timer = new QTimer(this);

    //创建界面布局元素
    create_main_layout();

    //界面操作触发
    connect(start_button, SIGNAL(clicked()), this, SLOT(start_close_handle()));
    connect(send_button, SIGNAL(clicked()), this, SLOT(send_stop_handle()));
    connect(count_clear_button, SIGNAL(clicked()), this, SLOT(count_clear_handle()));
    connect(type_box, SIGNAL(currentIndexChanged(int)), this, SLOT(current_typebox_change(int)));
    connect(peer_box, SIGNAL(currentTextChanged(QString)), this, SLOT(current_peerbox_change(QString)));

    connect(ahr_button_group, SIGNAL(buttonClicked(int)), this, SLOT(hex_recv_check_handle(int)));
    connect(ahs_button_group, SIGNAL(buttonClicked(int)), this, SLOT(hex_send_check_handle(int)));

    connect(clear_recv_button, SIGNAL(clicked()), this, SLOT(clear_recv_browser()));
    connect(clear_send_button, SIGNAL(clicked()), this, SLOT(clear_send_edit()));

    connect(lf_check, SIGNAL(stateChanged(int)), this, SLOT(lf_check_handle(int)));
    connect(file_recv_check, SIGNAL(stateChanged(int)), this, SLOT(file_recv_check_handle(int)));
    connect(file_send_check, SIGNAL(stateChanged(int)), this, SLOT(file_send_check_handle(int)));
    connect(loop_send_check, SIGNAL(stateChanged(int)), this, SLOT(loop_check_handle(int)));
    connect(loop_timer, SIGNAL(timeout()), this, SLOT(emit_loop_send()));
    loop_timer->start(10);
}

main_window::~main_window()
{
}


int main_window::start_handle()
{
    int ret = 0;

    //首先进行状态检查
    ip_addr = ip_edit->text();
    if(is_valid_ip(ip_addr.toStdString().c_str()) == -1){
        qDebug() << "Get ip failed.";
        QMessageBox::warning(this, tr("IP地址录入错误"), "请重新输入合法的IP地址.");
        return -1;
    }

    //获取端口号
    port = port_edit->text().toUShort();
    if((port < 1) || (port > 65534)){
        qDebug() << "Get port failed.";
        QMessageBox::warning(this, tr("端口号录入错误"), "请重新输入端口号，范围: 1~65534.");
        return -1;
    }

    //判断模式
    type_index = type_box->currentIndex();

    switch (type_index) {
        case TCP_CLIENT_INDEX:
            create_socket_client(ip_addr, port, PROTOCOL_TYPE_TCP);
            break;
        case TCP_SERVER_INDEX:
            ret = create_tcp_listen(ip_addr, port);
            break;
        case UDP_CLIENT_INDEX:
            create_socket_client(ip_addr, port, PROTOCOL_TYPE_UDP);
            break;
        case UDP_SERVER_INDEX:
            ret = create_udp_listen(ip_addr, port);
            break;
        case UDP_BROADCAST_INDEX:
            create_socket_client(ip_addr, port, PROTOCOL_TYPE_BROADCAST);
            break;
        case UDP_MULTICAST_INDEX:
            ret = create_multicast_join(ip_addr, port);
            break;
        default:
            break;
    }

    //如果是由文件接收的话,需要在此时打开接收文件
    if(recv_file_flag == NT_FLAG_YES){
        open_recv_file();
    }

    //如果是由文件发送的话,需要在此时打开发送文件
    if(send_file_flag == NT_FLAG_YES){
        open_send_file();
    }

    return ret;
}


void main_window::set_init_status()
{
    peer_box->clear();
    peer_box->repaint();
}

int main_window::close_handle()
{
    //如果是由文件接收的话,需要在此时关闭接收文件
    if(recv_file_flag == NT_FLAG_YES){
        close_recv_file();
    }

    //如果是由文件发送的话,需要在此时关闭发送文件
    if(send_file_flag == NT_FLAG_YES){

        close_send_file();
    }

    //如果正在循环发送的话,需要停止定时器
    stop_loop_send();

    if((type_index == TCP_CLIENT_INDEX) && (socket_client != nullptr)){
        socket_client->tcp_close_socket();

        delete  socket_client;
        socket_client = nullptr;
    }

    if((type_index == TCP_SERVER_INDEX) && (socket_server != nullptr)){
        session_hash->clear();
        socket_server->close_tcp_listen();
        delete  socket_server;
        socket_server = nullptr;

    }

    if((type_index == UDP_CLIENT_INDEX) && (socket_client != nullptr)){
        socket_client->udp_close_socket();

        delete  socket_client;
        socket_client = nullptr;
    }

    if((type_index == UDP_SERVER_INDEX) && (socket_server != nullptr)){
        session_hash->clear();
        socket_server->close_udp_listen();
        delete  socket_server;
        socket_server = nullptr;

    }

    if((type_index == UDP_BROADCAST_INDEX) && (socket_client != nullptr)){
        socket_client->udp_close_socket();

        delete  socket_client;
        socket_client = nullptr;
    }

    if((type_index == UDP_MULTICAST_INDEX) && (socket_server != nullptr)){
        session_hash->clear();
        delete  socket_server;
        socket_server = nullptr;
    }

    //设置初始状态
    set_init_status();

    return 0;
}


void main_window::start_close_handle()
{
    int ret;

    if(start_button->text() == "打开"){            //处于关闭状态的时候，用户点击开始按钮
        ret = start_handle();
        if(ret == 0){
            start_button->setText("关闭");
            start_button->repaint();
            set_setui_disable();
        }
    } else if(start_button->text() == "关闭"){    //处于开启状态的时候，用户点击关闭按钮
        ret = close_handle();
        if(ret == 0){
            start_button->setText("打开");
            start_button->repaint();
            set_setui_enable();
        }

    }
}


void main_window::update_status_connected(nt_session *session)
{
    statusBar()->showMessage(session->status_msg);

    session->socket = socket_client;

    //qDebug() << "update_status_connected session_key is:" << session->session_key;

    //新会话插入hash表
    session_hash->insert(session->session_key, session);

    //更新源地址显示
    peer_box->addItem(session->session_key);
    peer_box->repaint();

    //此函数只能被客户端调用, 客户端只有一个并发连接
    concurrent_print_data_label->setText("1");
}

void main_window::update_status_connect_error()
{
    statusBar()->showMessage("connect failed.");

    start_button->setText("打开");
    start_button->repaint();

    set_setui_enable();
}

void main_window::update_status_disconnected(QString session_key)
{
    QString msg = "disconnected form: " + session_key;
    statusBar()->showMessage(msg);

    session_hash->remove(session_key);

    for(int i = 0; i<peer_box->count(); i++){
        if(peer_box->itemText(i) == session_key){
            peer_box->removeItem(i);
        }
    }

    //如果是客户端的话设置为0,如果是服务端的话就不管了,服务端自会通知
    if((type_index == TCP_CLIENT_INDEX) && (socket_client != nullptr)){
        concurrent_print_data_label->setText("0");
        return;
    }
}


void main_window::update_count_recv(nt_session *session)
{
     recv_print_data_label->setText(QString::number(session->recv_count));
     recv_print_data_label->repaint();
}

void main_window::update_count_send(nt_session *session)
{
     send_print_data_label->setText(QString::number(session->send_count));
     send_print_data_label->repaint();
}


/*
 * 当收到用户修改对端对象时，要修ip显示的数字计信息
 */
void main_window::current_typebox_change(int index)
{
    switch (index) {
    case TCP_CLIENT_INDEX:
    case TCP_SERVER_INDEX:
    case UDP_CLIENT_INDEX:
    case UDP_SERVER_INDEX:
        ip_label->setText("IP地址");
        ip_label->repaint();
        break;
    case UDP_BROADCAST_INDEX:
        if(ip_edit->text() == DEFAULT_IP){
            ip_edit->setText(DEFAULT_BRO_IP);
            ip_edit->repaint();
        }
        ip_label->setText("广播地址");
        ip_label->repaint();
        break;
    case UDP_MULTICAST_INDEX:
        if(ip_edit->text() == DEFAULT_IP){
            ip_edit->setText(DEFAULT_MUL_IP);
            ip_edit->repaint();
        }
        ip_label->setText("组播地址");
        ip_label->repaint();
        break;
    default:
        break;
    }
}


/*
 * 当收到用户修改对端对象时，要修改统计信息
 */
void main_window::current_peerbox_change(QString box_text)
{
    nt_session *session = session_hash->value(box_text);

    if(session == nullptr){
        return;
    }

    nt_socket *socket = session->socket;

    recv_print_data_label->setText(QString::number(socket->current_session->recv_count));
    recv_print_data_label->repaint();

    send_print_data_label->setText(QString::number(socket->current_session->send_count));
    send_print_data_label->repaint();
}

void main_window::windows_clear_handle()
{
    recv_print_data_label->setText("0");
    recv_print_data_label->repaint();
    send_print_data_label->setText("0");
    send_print_data_label->repaint();
}

void main_window::count_clear_handle()
{
    if((type_index == TCP_CLIENT_INDEX) && (socket_client != nullptr)){
        socket_client->count_clear();
        return;
    }

    if((type_index == TCP_SERVER_INDEX) && (socket_server != nullptr)){
        nt_session *session = session_hash->value(peer_box->currentText());
        nt_socket *client_socket = session->socket;
        client_socket->count_clear();
        return;
    }

    if((type_index == UDP_CLIENT_INDEX) && (socket_client != nullptr)){
        socket_client->count_clear();
        return;
    }

    if((type_index == UDP_SERVER_INDEX) && (socket_server != nullptr)){
        nt_session *session = session_hash->value(peer_box->currentText());
        nt_socket *client_socket = session->socket;
        client_socket->count_clear();
        return;
    }

    if((type_index == UDP_BROADCAST_INDEX) && (socket_client != nullptr)){
        socket_client->count_clear();
        return;
    }

    if((type_index == UDP_MULTICAST_INDEX) && (socket_server != nullptr)){
        nt_session *session = session_hash->value(peer_box->currentText());
        nt_socket *client_socket = session->socket;
        client_socket->count_clear();
        return;
    }

    windows_clear_handle();
}

void main_window::show_data_handle(nt_session *session)
{
    if(recv_file_str.size() > 0){
        return;
    }

    QString perfix = "from session: " + session->session_key;

    if(lf_flag == NT_FLAG_YES){
        recv_browser->append(perfix);
    }

    //如果存在接收转文件，则先写文件,然后再输出显示
    if(recv_file_flag == NT_FLAG_YES){
        recv_to_file(session);
    }

    if(recv_hex_ascii_flag == NT_FLAG_HEX){
        QString out_tmp = session->recv_data.toHex(' ');
        recv_browser->append(out_tmp);
    } else {
        recv_browser->append(session->recv_data);
    }
}

void main_window::send_from_input(nt_session *session)
{
    QByteArray arry_text;
    //获取socket
    nt_socket *nt_socket = session->socket;

    QByteArray input_str = send_edit->toPlainText().toUtf8();

    if(recv_hex_ascii_flag == NT_FLAG_HEX){
        arry_text = QByteArray::fromHex(input_str);
    } else {
        arry_text = input_str;
    }

    //由页面获取
    if(input_str.size() == 0){
        QMessageBox::warning(this, tr("请输入发送数据"), "发送数据不可以为空.");
        return;
    }

    if((type_index == TCP_CLIENT_INDEX) && (socket_client != nullptr)){
        nt_socket->tcp_client_send(arry_text);
        return;
    }

    if((type_index == TCP_SERVER_INDEX) && (socket_server != nullptr)){
        nt_socket->tcp_client_send(arry_text);
        return;
    }

    if((type_index == UDP_CLIENT_INDEX) && (socket_client != nullptr)){
        nt_socket->udp_client_send(arry_text);
        return;
    }

    if((type_index == UDP_SERVER_INDEX) && (socket_server != nullptr)){
        nt_socket->udp_client_send(arry_text);
        return;
    }

    if((type_index == UDP_BROADCAST_INDEX) && (socket_client != nullptr)){
        nt_socket->udp_client_send(arry_text);
        return;
    }

    if((type_index == UDP_MULTICAST_INDEX) && (socket_server != nullptr)){
        nt_socket->udp_client_send(arry_text);
        return;
    }
}

void main_window::emit_loop_send()
{
    if(record_for_loop != nullptr){
        send_from_input(record_for_loop);
    }
}

void main_window::stop_loop_send()
{
    loop_timer->stop();

    send_button->setText("发送");
    send_button->repaint();
}



bool main_window::send_to_peer(nt_session *session)
{
    //由文件获取输入数据
    if(send_file_flag == NT_FLAG_YES){
        send_from_file(session);
        return true;
    }

    //如果是循环发送，则设置循环间隔
    if(loop_send_flag == NT_FLAG_YES){
        loop_time = loop_time_edit->text().toInt();
        if((loop_time < LOOP_MIN_TIME) || (loop_time > LOOP_MAX_TIME)){
            QMessageBox::warning(this, tr("设置循环事件错误"), "循环事件只能设置为10~10000以内.");
            return false;
        }

        loop_timer->start(loop_time);
        record_for_loop = session;
    }

    send_from_input(session);

    return true;
}

bool main_window::send_handle()
{
    //首先获取当前的session_key
    QString current_key = peer_box->currentText();

    if(socket_client == nullptr && socket_server == nullptr){
        QMessageBox::warning(this, tr("当前没有连接啊"), "你必须得建立连接之后才能发送数据.");
        return false;
    }

    QHash<QString, nt_session *>::const_iterator socket_it;
    for (socket_it = session_hash->begin(); socket_it != session_hash->end(); ++socket_it) {
        if(current_key == socket_it.key()) {
            send_to_peer(socket_it.value());
        }
    }

    return true;
}

void main_window::send_stop_handle()
{

    if(send_button->text() == "发送"){
        //如果是循环的
        if(loop_send_flag == NT_FLAG_YES){
            send_button->setText("停止");
            send_button->repaint();
        }
        send_handle();
    } else if(send_button->text() == "停止"){
        stop_loop_send();
    }
}

void main_window::update_count_con(int count)
{
    concurrent_print_data_label->setText(QString::number(count));
}

void main_window::clear_recv_browser()
{
    recv_browser->clear();
    recv_browser->repaint();
}

void main_window::clear_send_edit()
{
    send_edit->clear();
    send_edit->repaint();
}

void main_window::hex_recv_check_handle(int state)
{
    if(state == NT_FLAG_HEX){
        recv_hex_ascii_flag = NT_FLAG_HEX;
    }

    if(state == NT_FLAG_ASCII){
        recv_hex_ascii_flag = NT_FLAG_ASCII;
    }
}

void main_window::hex_send_check_handle(int state)
{
    if(send_hex_ascii_flag == NT_FLAG_ASCII){
        if(state == NT_FLAG_HEX){
            send_hex_ascii_flag = NT_FLAG_HEX;

            //如果当前输入框有数据，则将数据转为十六进制数据
            QByteArray input_str = send_edit->toPlainText().toUtf8();
            if(input_str.size() > 0){
                QString out_tmp = input_str.toHex(' ');
                send_edit->setText(out_tmp);
                send_edit->repaint();
            }

        }
    }

    if(send_hex_ascii_flag == NT_FLAG_HEX){
        if(state == NT_FLAG_ASCII){
            send_hex_ascii_flag = NT_FLAG_ASCII;

            //如果当前输入框有数据，则将数据转为ascii数据
            QByteArray input_str = send_edit->toPlainText().toUtf8();
            if(input_str.size() > 0){
                QString out_tmp = input_str.fromHex(input_str);
                send_edit->setText(out_tmp);
                send_edit->repaint();
            }

        }
    }
}


void main_window::lf_check_handle(int state)
{
    // "选中"
    if (state == Qt::Checked) {
        lf_flag = NT_FLAG_YES;
    } else  {
        lf_flag = NT_FLAG_NO;
    }
}

void main_window::loop_check_handle(int state)
{
    // "选中"
    if (state == Qt::Checked) {
        loop_send_flag = NT_FLAG_YES;
    } else  {
        loop_send_flag = NT_FLAG_NO;
    }
}
