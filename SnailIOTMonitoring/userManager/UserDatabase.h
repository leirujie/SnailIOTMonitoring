#ifndef DATABAS_H
#define DATABAS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

class UserDatabase : public QWidget
{
    Q_OBJECT
public:
    explicit UserDatabase(QWidget *parent = nullptr);
    ~UserDatabase();
    // 保存用户信息
    void saveUserInfo(const QString &username, const QString &password, const QString &email, const QString &phone, const QString &nickname, const QString &role);
    // 删除指定用户名的用户信息
    void deleteUserInfo(const QString &username);
    // 更新指定用户名的密码
    void updateUserPassword(const QString &username, const QString &newPassword);
    // 根据用户名查询用户信息，返回包含用户各字段信息的QMap（若找到），若没找到返回空QMap
    QMap<QString, QString> queryUserInfo(const QString &username);
private:
    QSqlDatabase m_db;
    // 打开和关闭数据库连接
    bool openDatabase();
    void closeDatabase(QSqlDatabase &db);
};

#endif // DATABAS_H
