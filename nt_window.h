/*
 * Copyright (C) miaogx@aliyun.com
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H


#include "nt_include.h"
#include "nt_socket.h"
#include "nt_server.h"
#include "nt_session.h"


#define TCP_CLIENT_STR        "TCP客户端"
#define TCP_SERVER_STR        "TCP服务端"
#define UDP_CLIENT_STR        "UDP客户端"
#define UDP_SERVER_STR        "UDP服务端"
#define UDP_BROADCAST_STR     "UDP广播"
#define UDP_MULICAST_STR      "UDP组播"


#define TCP_CLIENT_INDEX       0
#define TCP_SERVER_INDEX       1
#define UDP_CLIENT_INDEX       2
#define UDP_SERVER_INDEX       3
#define UDP_BROADCAST_INDEX    4
#define UDP_MULTICAST_INDEX    5

#define NT_FLAG_NO             0
#define NT_FLAG_YES            1

#define LOOP_MIN_TIME          10
#define LOOP_MAX_TIME          100000

#define NT_FLAG_ASCII          1
#define NT_FLAG_HEX            2


class main_window : public QMainWindow
{
    Q_OBJECT

private:

    QHBoxLayout *main_layout;
    QVBoxLayout *set_layout;
    QVBoxLayout *data_layout;

    //网络设置组
    QGroupBox *set_net_group;
    QVBoxLayout *set_net_layout;
    QLabel *type_label;
    QComboBox *type_box;
    QLabel *ip_label;
    QLineEdit *ip_edit;
    QLabel *port_label;
    QLineEdit *port_edit;
    QPushButton *start_button;

    //接收设置组
    QGroupBox *set_recv_group;
    QVBoxLayout *set_recv_layout;
    QButtonGroup *ahr_button_group;
    QRadioButton *ar_button;
    QRadioButton *hr_button;

    QCheckBox *lf_check;
    QCheckBox *file_recv_check;
    QPushButton *clear_recv_button;

    //发送设置组
    QGroupBox *set_send_group;
    QVBoxLayout *set_send_layout;
    QButtonGroup *ahs_button_group;
    QRadioButton *as_button;
    QRadioButton *hs_button;

    QCheckBox *file_send_check;
    QCheckBox *loop_send_check;
    QPushButton *clear_send_button;
    QLabel *loop_time_label;
    QLineEdit *loop_time_edit;


    //数据接收组
    QGroupBox *data_recv_group;
    QVBoxLayout *data_recv_layout;
    QTextBrowser *recv_browser;

    //数据发送组
    QGroupBox *data_send_group;
    QHBoxLayout *data_send_layout;
    QVBoxLayout *start_layout;
    QComboBox *peer_box;
    QPushButton *send_button;
    QTextEdit *send_edit;

    //数据统计组
    QGroupBox *data_count_group;
    QHBoxLayout *count_layout;
    QPushButton *count_clear_button;
    QLabel *concurrent_print_label;
    QLabel *concurrent_print_data_label;
    QLabel *recv_print_label;
    QLabel *recv_print_data_label;
    QLabel *send_print_label;
    QLabel *send_print_data_label;


    QString ip_addr;
    quint16 port;

    int send_stop_flag;

    int lf_flag;
    int recv_hex_ascii_flag;
    int send_hex_ascii_flag;
    int recv_file_flag;
    int send_file_flag;
    int loop_send_flag;
    int loop_time;
    QTimer *loop_timer;
    nt_session *record_for_loop;

    QString recv_file_str;
    QFile recv_file;
    QTextStream recv_file_stream;

    QString send_file_str;
    QFile send_file;
    QTextStream send_file_stream;

    nt_socket *socket_client;
    nt_server *socket_server;

    //存储TCP socket 的hash
    QHash<QString, nt_session *> *session_hash;

public:
    main_window(QWidget *parent = nullptr);
    ~main_window();

    void create_main_layout();
    void windows_clear_handle();
    void server_create_status_bar();

    QGroupBox *create_set_net_group();
    QGroupBox *create_set_recv_group();
    QGroupBox *create_set_send_group();

    QGroupBox *create_data_recv_group();
    QGroupBox *create_data_count_group();
    QGroupBox *create_data_send_group();

    int type_index;

    void create_socket_client(QString ip, quint16 port, int type);

    int create_tcp_listen(QString ip, quint16 port);
    int create_udp_listen(QString ip, quint16 port);
    int create_multicast_listen(QString ip, quint16 port);

    int start_handle();
    int close_handle();

    bool send_to_peer(nt_session *session);

    bool create_recv_file();
    void close_recv_file();

    bool open_recv_file();
    bool open_send_file();

    void set_setui_disable();
    void set_setui_enable();

    bool send_handle();

    void send_from_input(nt_session *session);
    int send_from_file(nt_session *session);
    int recv_to_file(nt_session *session);

    void stop_loop_send();

signals:


public slots:
    void update_status_connected(nt_session *session);
    void update_status_disconnected(QString session_key);
    void current_typebox_change(int index);
    void current_peerbox_change(QString box_text);
    void start_close_handle();

    void send_stop_handle();

    void update_count_recv(nt_session *session);
    void update_count_send(nt_session *session);
    void update_count_con(int count);
    void count_clear_handle();

    void show_data_handle(nt_session *session);

    void listen_successful(QString msg);
    void listen_failed(QString msg);
    void new_connected(nt_session *session);

    void clear_recv_browser();
    void clear_send_edit();

    void hex_recv_check_handle(int state);
    void hex_send_check_handle(int state);

    void lf_check_handle(int);
    void loop_check_handle(int);
    void file_recv_check_handle(int state);
    void file_send_check_handle(int state);

    void emit_loop_send();

};
#endif // MAIN_WINDOW_H

