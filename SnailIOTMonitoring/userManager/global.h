#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QMutex>

// 定义全局变量用于保存当前登录用户名
extern QString g_currentUsername ;
// 定义互斥锁用于保护对全局变量的访问
extern QMutex g_usernameMutex;


#endif
