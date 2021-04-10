/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_window.h"

void main_window::create_main_layout()
{
    server_create_status_bar();

    main_layout = new QHBoxLayout;
    set_layout = new QVBoxLayout;
    data_layout = new QVBoxLayout;

    font_set.setBold(true);

    set_layout->addWidget(create_set_net_group());
    set_layout->addWidget(create_set_recv_group());
    set_layout->addWidget(create_set_send_group());
    data_layout->addWidget(create_data_recv_group());

    data_layout->addWidget(create_data_send_group());
    data_layout->addWidget(create_data_count_group());

    data_layout->setStretchFactor(data_recv_group, 3);
    data_layout->setStretchFactor(data_send_group, 1);

    main_layout->addLayout(set_layout);
    main_layout->addLayout(data_layout);
    main_layout->setStretchFactor(set_layout, 1);
    main_layout->setStretchFactor(data_layout, 4);

    QWidget* widget = new QWidget();
    widget->setLayout(main_layout);
    this->setCentralWidget(widget);

    //setWindowIcon(QIcon(":/images/nt_for_mac.icns"));
    setWindowIcon(QIcon("qrc:/images/nt_for_mac.icns"));
}


QGroupBox *main_window::create_set_net_group()
{
    set_net_group = new QGroupBox("基础信息");
    //font_set.setPointSize(20);
    //set_net_group->setT
    set_net_layout = new QVBoxLayout;

    type_label = new QLabel("协议类型", this);
    type_box = new QComboBox;

    QStringList strList;
    strList << TCP_CLIENT_STR << TCP_SERVER_STR << UDP_CLIENT_STR << UDP_SERVER_STR << UDP_BROADCAST_STR << UDP_MULICAST_STR;
    type_box->addItems(strList);

    ip_label = new QLabel("ip 地址", this);
    ip_edit = new QLineEdit;
    ip_edit->setText(DEFAULT_IP);          //设置个默认IP

    port_label = new QLabel("端口号", this);
    port_edit = new QLineEdit;
    port_edit->setText(DEFAULT_PORT);             //设置个默认端口号
    start_button = new QPushButton("打开");

    set_net_layout->addWidget(type_label);
    set_net_layout->addWidget(type_box);
    set_net_layout->addWidget(ip_label);
    set_net_layout->addWidget(ip_edit);
    set_net_layout->addWidget(port_label);
    set_net_layout->addWidget(port_edit);
    set_net_layout->addWidget(start_button);

    set_net_group->setLayout(set_net_layout);

    return set_net_group;
}

QGroupBox *main_window::create_set_recv_group()
{
    set_recv_group = new QGroupBox("接收设置");
    set_recv_layout = new QVBoxLayout();

    QHBoxLayout *ah_layout = new QHBoxLayout();
    ahr_button_group = new QButtonGroup(this);
    ar_button = new QRadioButton(this);
    ar_button->setText("ASCII");

    hr_button = new QRadioButton(this);
    hr_button->setText("HEX");

    ar_button->setChecked(1);
    ahr_button_group->setExclusive(true);        //设置互斥

    lf_check = new QCheckBox("显示接收源");
    file_recv_check = new QCheckBox("接收转文件");
    clear_recv_button = new QPushButton("清除接收");

    ah_layout->addWidget(ar_button);
    ahr_button_group->addButton(ar_button);
    ahr_button_group->setId(ar_button, NT_FLAG_ASCII);

    ah_layout->addWidget(hr_button);
    ahr_button_group->addButton(hr_button);
    ahr_button_group->setId(hr_button, NT_FLAG_HEX);

    set_recv_layout->addLayout(ah_layout);
    set_recv_layout->addWidget(lf_check);
    set_recv_layout->setSpacing(0);
    set_recv_layout->addWidget(file_recv_check);
    set_recv_layout->addWidget(clear_recv_button);

    set_recv_group->setLayout(set_recv_layout);

    return set_recv_group;
}

QGroupBox *main_window::create_set_send_group()
{
    set_send_group = new QGroupBox("发送设置");
    set_send_layout = new QVBoxLayout();

    QHBoxLayout *ah_layout = new QHBoxLayout();
    ahs_button_group = new QButtonGroup(this);
    as_button = new QRadioButton(this);
    as_button->setText("ASCII");

    hs_button = new QRadioButton(this);
    hs_button->setText("HEX");

    as_button->setChecked(1);
    ahs_button_group->setExclusive(true);       //设置互斥


    file_send_check = new QCheckBox("从文件发送");
    clear_send_button = new QPushButton("清除发送");
    loop_send_check = new QCheckBox("循环发送");

    QHBoxLayout *loop_layout = new QHBoxLayout();
    loop_time_label = new QLabel("ms");
    loop_time_edit = new QLineEdit;
    loop_layout->addWidget(loop_time_edit);
    loop_layout->addWidget(loop_time_label);

    ah_layout->addWidget(as_button);
    ahs_button_group->addButton(as_button);
    ahs_button_group->setId(as_button, NT_FLAG_ASCII);

    ah_layout->addWidget(hs_button);
    ahs_button_group->addButton(hs_button);
    ahs_button_group->setId(hs_button, NT_FLAG_HEX);

    set_send_layout->addLayout(ah_layout);
    set_send_layout->addWidget(file_send_check);
    set_send_layout->addWidget(loop_send_check);
    set_send_layout->addLayout(loop_layout);
    set_send_layout->addWidget(clear_send_button);

    set_send_group->setLayout(set_send_layout);

    return set_send_group;
}


QGroupBox *main_window::create_data_recv_group()
{
    data_recv_group = new QGroupBox("数据接收");

    data_recv_layout = new QVBoxLayout();
    recv_browser = new QTextBrowser();

    data_recv_layout->addWidget(recv_browser);
    data_recv_group->setLayout(data_recv_layout);

    return data_recv_group;
}

QGroupBox *main_window::create_data_send_group()
{
    data_send_group = new QGroupBox("数据发送");

    data_send_layout = new QHBoxLayout();
    start_layout = new QVBoxLayout();
    send_edit = new QTextEdit();
    send_edit->setText(SEND_DATA_DEFAULT);
    peer_box = new QComboBox;

    send_button = new QPushButton("发送");

    start_layout->addWidget(peer_box);
    start_layout->addWidget(send_button);
    start_layout->setStretchFactor(peer_box, 1);
    start_layout->setStretchFactor(send_button, 2);

    data_send_layout->addWidget(send_edit);
    data_send_layout->addLayout(start_layout);
    data_send_layout->setStretchFactor(send_edit, 2);
    data_send_layout->setStretchFactor(start_layout, 1);

    data_send_group->setLayout(data_send_layout);

    return data_send_group;
}

QGroupBox *main_window::create_data_count_group()
{
    data_count_group = new QGroupBox("数据统计");

    count_layout = new QHBoxLayout();

    count_clear_button = new QPushButton("计数清零");
    concurrent_print_label = new QLabel("con:");
    concurrent_print_data_label = new QLabel("0");
    recv_print_label = new QLabel("recv:");
    recv_print_data_label = new QLabel("0");
    send_print_label = new QLabel("send:");
    send_print_data_label = new QLabel("0");

    count_layout->addWidget(count_clear_button);
    count_layout->addStretch();
    count_layout->addWidget(concurrent_print_label);
    count_layout->addWidget(concurrent_print_data_label);
    count_layout->addWidget(recv_print_label);
    count_layout->addWidget(recv_print_data_label);
    count_layout->addWidget(send_print_label);
    count_layout->addWidget(send_print_data_label);
    //count_layout->setSpacing(1);

    data_count_group->setLayout(count_layout);

    return data_count_group;
}

void main_window::server_create_status_bar()
{
    statusBar()->showMessage("Status is here...");
}


void main_window::set_setui_disable()
{
    //网络设置组
    type_box->setEnabled(false);
    type_box->repaint();
    ip_edit->setEnabled(false);
    ip_edit->repaint();
    port_edit->setEnabled(false);
    port_edit->repaint();

    //接收设置组
    ar_button->setEnabled(false);
    ar_button->repaint();
    hr_button->setEnabled(false);
    hr_button->repaint();

    lf_check->setEnabled(false);
    lf_check->repaint();
    file_recv_check->setEnabled(false);
    file_recv_check->repaint();

    //发送设置组
    as_button->setEnabled(false);
    as_button->repaint();
    hs_button->setEnabled(false);
    hs_button->repaint();

    file_send_check->setEnabled(false);
    file_send_check->repaint();
    loop_send_check->setEnabled(false);
    loop_send_check->repaint();
    loop_time_edit->setEnabled(false);
    loop_time_edit->repaint();
}

void main_window::set_setui_enable()
{
    //网络设置组
    type_box->setEnabled(true);
    type_box->repaint();
    ip_edit->setEnabled(true);
    ip_edit->repaint();
    port_edit->setEnabled(true);
    port_edit->repaint();

    //接收设置组
    ar_button->setEnabled(true);
    ar_button->repaint();
    hr_button->setEnabled(true);
    hr_button->repaint();

    lf_check->setEnabled(true);
    lf_check->repaint();
    file_recv_check->setEnabled(true);
    file_recv_check->repaint();

    //发送设置组
    as_button->setEnabled(true);
    as_button->repaint();
    hs_button->setEnabled(true);
    hs_button->repaint();

    file_send_check->setEnabled(true);
    file_send_check->repaint();
    loop_send_check->setEnabled(true);
    loop_send_check->repaint();
    loop_time_edit->setEnabled(true);
    loop_time_edit->repaint();
}
