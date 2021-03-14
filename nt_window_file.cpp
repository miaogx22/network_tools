/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_window.h"

bool main_window::create_recv_file()
{
    /*
    getSaveFileName的参数说明
    QString QFileDialog::getSaveFileName(
        QWidget *parent = nullptr,               //指定父窗口
        const QString &caption = QString(),      //对话框的标题
        const QString &dir = QString(),          //默认的打开目录
        const QString &filter = QString(),       //对话框的后缀名过滤器，多个文件用空格分隔
        QString *selectedFilter = nullptr,       //默认选择的过滤器
        QFileDialog::Options options = Options() //额外的参数设置
    )

*/
    QString default_path = QDir::homePath();
    recv_file_str = QFileDialog::getSaveFileName(this, "请选择一个存储文件来存储接收到的数据", default_path
                     );

    QFile *rfp = new QFile(recv_file_str);

    if(rfp->open(QIODevice::ReadWrite)){
        QString msg = "create recv data file: " + recv_file_str + " successful.";
        statusBar()->showMessage(msg);
        rfp->close();
    }

    return true;
}

bool main_window::open_recv_file()
{
    recv_file.setFileName(recv_file_str);

    if (!recv_file.exists()){
        QMessageBox::warning(this, tr("警告"), "你选择的文件并不存在啊.");
        return false;
    }

    if (!recv_file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("警告"), "打开你选择的文件出错了.");
        return false;
    }

    QString msg = "open recv data file: " + recv_file_str + " successful.";
    statusBar()->showMessage(msg);

    return true;
}

void main_window::file_recv_check_handle(int state)
{
    // "选中"
    if (state == Qt::Checked) {
        recv_file_flag =NT_FLAG_YES;
        create_recv_file();
    } else  {
        recv_file_flag = NT_FLAG_NO;
    }
}

bool main_window::open_send_file()
{
    QString default_path = QDir::homePath();
    send_file_str = QFileDialog::getOpenFileName(this, "请选择你要发送数据的存储文件, 注意，此文件必须为文本文件", default_path);

    send_file.setFileName(send_file_str);

    if (!send_file.exists()){
        QMessageBox::warning(this, tr("警告"), "你选择的文件并不存在啊.");
        return false;
    }

    if (!send_file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("警告"), "打开你选择的文件出错了.");
        return false;
    }

    return true;
}

void main_window::file_send_check_handle(int state)
{
    // "选中"
    if (state == Qt::Checked) {
        send_file_flag =NT_FLAG_YES;
        open_send_file();
    } else  {
        send_file_flag = NT_FLAG_NO;
    }
}

int main_window::send_from_file(nt_session *session)
{
    //获取socket
    nt_socket *nt_socket = session->socket;

    QTextStream send_file_stream(&send_file);       //用文本流读取文件
    send_file_stream.setAutoDetectUnicode(true);    //自动检测Unicode,才能正常显示文档内的汉字

    while (!send_file_stream.atEnd()){
        QString str = send_file_stream.readLine();  //读取文件的一行
        if((type_index == TCP_CLIENT_INDEX) && (socket_client != nullptr)){
            nt_socket->tcp_client_send(str.toUtf8());
        }

        if((type_index == TCP_SERVER_INDEX) && (socket_server != nullptr)){
            nt_socket->tcp_client_send(str.toUtf8());
        }
    }
    send_file.close();

    return 0;
}

int main_window::recv_to_file(nt_session *session)
{
    QTextStream recv_file_stream(&recv_file);       //用文本流读取文件
    recv_file_stream.setAutoDetectUnicode(true);    //自动检测Unicode,才能正常显示文档内的汉字

    recv_file_stream << session->recv_data;

    return 0;
}

void main_window::close_recv_file()
{
    recv_file.close();
}
