/*
 * Copyright (C) miaogx@aliyun.com
 */

#include "nt_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //实则默认体大小
    QFont font;
    font.setPointSize(12);
    a.setFont(font);

    main_window w;
    w.show();
    return a.exec();
}
