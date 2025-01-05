#include "UserDatabase.h"

UserDatabase::UserDatabase(QWidget *parent) : QWidget(parent)
{
}

UserDatabase::~UserDatabase()
{
    closeDatabase(m_db);
}

// 打开数据库连接
bool UserDatabase::openDatabase()
{
    if (!m_db.isValid()) {
        m_db = QSqlDatabase::addDatabase("QSQLITE", " ");
        m_db.setDatabaseName("userLogin.db");
    }
    return m_db.open();
}

// 保存用户信息
void UserDatabase::saveUserInfo(const QString &username, const QString &password, const QString &email, const QString &phone, const QString &nickname, const QString &role)
{
    if (openDatabase()) {
        QSqlQuery query(m_db);
        query.exec("CREATE TABLE IF NOT EXISTS users (user_id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE NOT NULL, password TEXT NOT NULL, email TEXT UNIQUE NOT NULL, phone TEXT UNIQUE NOT NULL, nickname TEXT, role TEXT NOT NULL)");
        query.prepare("INSERT INTO users (username, password, email, phone, nickname, role) VALUES (:username, :password, :email, :phone, :nickname, :role)");
        query.bindValue(":username", username);
        query.bindValue(":password", password);
        query.bindValue(":email", email);
        query.bindValue(":phone", phone);
        query.bindValue(":nickname", nickname);
        query.bindValue(":role", role);
        query.exec();
        closeDatabase(m_db);
    }
}

// 删除指定用户名的用户信息函数
void UserDatabase::deleteUserInfo(const QString &username)
{
    if (openDatabase()) {
        QSqlQuery query(m_db);
        query.prepare("DELETE FROM users WHERE username = :username");
        query.bindValue(":username", username);
        query.exec();
        closeDatabase(m_db);
    }
}

// 更新指定用户名的密码函数
void UserDatabase::updateUserPassword(const QString &username, const QString &newPassword)
{
    if (openDatabase()) {
        QSqlQuery query(m_db);
        query.prepare("UPDATE users SET password = :password WHERE username = :username");
        query.bindValue(":username", username);
        query.bindValue(":password", newPassword);
        query.exec();
        closeDatabase(m_db);
    }
}

// 根据用户名查询用户信息，返回包含用户各字段信息的QMap（若找到），若没找到返回空QMap的函数
QMap<QString, QString> UserDatabase::queryUserInfo(const QString &username)
{
    QMap<QString, QString> userInfo;
    if (openDatabase()) {
        QSqlQuery query(m_db);
        query.prepare("SELECT username, password, email, phone, nickname, role FROM users WHERE username = :username");
        query.bindValue(":username", username);
        query.exec();
        if (query.next()) {
            userInfo["username"] = query.value(0).toString();
            userInfo["password"] = query.value(1).toString();
            userInfo["email"] = query.value(2).toString();
            userInfo["phone"] = query.value(3).toString();
            userInfo["nickname"] = query.value(4).toString();
            userInfo["role"] = query.value(5).toString();
        }
        closeDatabase(m_db);
    }
    return userInfo;
}

// 关闭数据库连接的函数
void UserDatabase::closeDatabase(QSqlDatabase &db)
{
    if (db.isOpen()) {
        db.close();
    }
}
