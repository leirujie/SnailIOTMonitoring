#include "UserDatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QMessageBox>

// 静态成员变量，用于保存单例实例指针
UserDatabase* UserDatabase::getInstance(QWidget *parent)
{
    static UserDatabase instance(parent);
    return &instance;
}

UserDatabase::UserDatabase(QWidget *parent) : QWidget(parent)
{
    openDatabase();
    createTableIfNotExists();
}

UserDatabase::~UserDatabase()
{
    closeDatabase();
}

// 打开数据库连接
bool UserDatabase::openDatabase()
{
    if (!m_db.isOpen()) {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("SnailIOTMonitor.db");
        if (!m_db.open()) {
            logError("Error opening database:", m_db.lastError());
            return false;
        }
    }
    return true;
}

// 关闭数据库连接
void UserDatabase::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

// 创建数据库表（如果不存在）
void UserDatabase::createTableIfNotExists()
{
    QSqlQuery query(m_db);
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE NOT NULL, "
               "password TEXT NOT NULL, "
               "email TEXT UNIQUE NOT NULL, "
               "phone TEXT UNIQUE NOT NULL, "
               "nickname TEXT, "
               "role TEXT NOT NULL)");
}

bool UserDatabase::checkUserExistence(const QString &username, const QString &phone, const QString &email)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username OR phone = :phone OR email = :email");
    query.bindValue(":username", username);
    query.bindValue(":phone", phone);
    query.bindValue(":email", email);
    query.exec();
    query.next();

    if (query.value(0).toInt() > 0) {
        qDebug() << "用户名、手机号或邮箱已存在";
        return true;
    }
    return false;
}


// 保存用户信息
void UserDatabase::saveUserInfo(const QString &username, const QString &password, const QString &email, const QString &phone, const QString &nickname, const QString &role)
{
    // 检查是否有相同的用户名、手机号、邮箱
    if (checkUserExistence(username, phone, email)) {
        return;
    }

    QSqlQuery query(m_db);
    m_db.transaction();  // 开启事务

    query.prepare("INSERT INTO users (username, password, email, phone, nickname, role) "
                  "VALUES (:username, :password, :email, :phone, :nickname, :role)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);
    query.bindValue(":nickname", nickname);
    query.bindValue(":role", role);

    if (!query.exec()) {
        m_db.rollback();  // 如果插入失败，回滚事务
        logError("Error saving user info:", query.lastError());
    } else {
        m_db.commit();  // 提交事务
        qDebug() << "User info saved successfully!";
    }
}



// 删除指定用户名的用户信息
void UserDatabase::deleteUserInfo(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        logError("Error deleting user:", query.lastError());
    }
}

// 更新用户密码
bool UserDatabase::updateUserInfo(const QString &username, const QString &newPassword)
{
    // 验证用户名不能为空
    if (username.isEmpty()) {
        qDebug() << "用户名不能为空，请检查输入。";
        return false;
    }

    // 验证密码不能为空且长度符合一定要求
    if (newPassword.isEmpty()) {
        qDebug() << "密码不能为空，请输入新密码。";
        return false;
    }
    if (newPassword.length() < 8) {
        qDebug() << "密码长度至少应为8位，请重新输入合适的密码。";
        return false;
    }

    // 构造 SQL 查询进行更新密码
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET password = :password WHERE username = :username");
    query.bindValue(":username", username);
    query.bindValue(":password", newPassword);

    if (!query.exec()) {
          // 可以记录错误信息或进行其他错误处理
          qDebug() << "更新密码失败，错误信息：" << query.lastError();
          return false;
      }

      // 检查是否有行被更新
      return query.numRowsAffected() > 0;
}


// 更新用户信息
void UserDatabase::updateUserInfo(const QString &username,
                                  const QString &password,
                                  const QString &email,
                                  const QString &phone,
                                  const QString &nickname,
                                  const QString &role)
{
    // 验证用户名不能为空
    if (username.isEmpty()) {
        qDebug() << "用户名不能为空，请检查输入。";
        return;
    }

    // 验证密码不能为空且长度符合一定要求
    if (!password.isEmpty() && password.length() < 8) {
        qDebug() << "密码长度至少应为8位，请重新输入合适的密码。";
        return;
    }

    // 验证邮箱格式
    QRegularExpression emailRegex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    if (!email.isEmpty() && !emailRegex.match(email).hasMatch()) {
        qDebug() << "邮箱格式不正确，请输入有效的邮箱地址。";
        return;
    }

    // 验证手机号格式（如果手机号不为空，验证格式）
    if (!phone.isEmpty()) {
        QRegularExpression phoneRegex("^1[3-9]\\d{9}$"); // 中国大陆手机号验证
        if (!phoneRegex.match(phone).hasMatch()) {
            qDebug() << "手机号格式不正确，请输入有效的手机号。";
            return;
        }

        // 检查手机号是否已经存在
        QSqlQuery checkPhoneQuery(m_db);
        checkPhoneQuery.prepare("SELECT COUNT(*) FROM users WHERE phone = :phone AND username != :username");
        checkPhoneQuery.bindValue(":phone", phone);
        checkPhoneQuery.bindValue(":username", username);
        checkPhoneQuery.exec();
        checkPhoneQuery.next();

        if (checkPhoneQuery.value(0).toInt() > 0) {
            qDebug() << "该手机号已经被其他用户注册，请更换手机号。";
            return;
        }
    }

    // 构造 SQL 查询进行更新
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET password = :password, email = :email, phone = :phone, nickname = :nickname, role = :role WHERE username = :username");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);
    query.bindValue(":nickname", nickname);
    query.bindValue(":role", role);

    // 执行查询并检查是否成功
    if (!query.exec()) {
        qDebug() << "更新用户信息失败，错误信息：" << query.lastError().text();
    } else {
        qDebug() << "用户信息更新成功。";
    }
}

// 更新用户信息（不修改用户角色）
void UserDatabase::updateUserInfo(const QString &username,
                                  const QString &password,
                                  const QString &email,
                                  const QString &phone,
                                  const QString &nickname)
{
    // 验证用户名不能为空
    if (username.isEmpty()) {
        qDebug() << "用户名不能为空，请检查输入。";
        return;
    }

    // 验证密码不能为空且长度符合一定要求
    if (!password.isEmpty() && password.length() < 8) {
        qDebug() << "密码长度至少应为8位，请重新输入合适的密码。";
        return;
    }

    // 验证邮箱格式
    QRegularExpression emailRegex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    if (!email.isEmpty() && !emailRegex.match(email).hasMatch()) {
        qDebug() << "邮箱格式不正确，请输入有效的邮箱地址。";
        return;
    }

    // 验证手机号格式（如果手机号不为空，验证格式）
    if (!phone.isEmpty()) {
        QRegularExpression phoneRegex("^1[3-9]\\d{9}$"); // 中国大陆手机号验证
        if (!phoneRegex.match(phone).hasMatch()) {
            qDebug() << "手机号格式不正确，请输入有效的手机号。";
            return;
        }

        // 检查手机号是否已经存在
        QSqlQuery checkPhoneQuery(m_db);
        checkPhoneQuery.prepare("SELECT COUNT(*) FROM users WHERE phone = :phone AND username != :username");
        checkPhoneQuery.bindValue(":phone", phone);
        checkPhoneQuery.bindValue(":username", username);
        checkPhoneQuery.exec();
        checkPhoneQuery.next();

        if (checkPhoneQuery.value(0).toInt() > 0) {
            qDebug() << "该手机号已经被其他用户注册，请更换手机号。";
            return;
        }
    }

    // 构造 SQL 查询进行更新（不更新角色）
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET password = :password, email = :email, phone = :phone, nickname = :nickname WHERE username = :username");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);
    query.bindValue(":nickname", nickname);

    // 执行查询并检查是否成功
    if (!query.exec()) {
        qDebug() << "更新用户信息失败，错误信息：" << query.lastError().text();
    } else {
        qDebug() << "用户信息更新成功。";
    }
}

// 根据用户名查询用户信息
QMap<QString, QString> UserDatabase::queryUserInfo(const QString &username)
{
    QMap<QString, QString> userInfo;
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
    return userInfo;
}

// 查询所有用户信息，返回包含所有用户各字段信息的QMap列表
QList<QMap<QString, QString>> UserDatabase::queryAllUserInfo()
{
    QList<QMap<QString, QString>> allUsersInfo;
    QSqlQuery query(m_db);
    query.exec("SELECT username, password, email, phone, nickname, role FROM users");
    while (query.next()) {
        QMap<QString, QString> userInfo;
        userInfo["username"] = query.value(0).toString();
        userInfo["password"] = query.value(1).toString();
        userInfo["email"] = query.value(2).toString();
        userInfo["phone"] = query.value(3).toString();
        userInfo["nickname"] = query.value(4).toString();
        userInfo["role"] = query.value(5).toString();
        allUsersInfo.append(userInfo);
    }
    return allUsersInfo;
}

// 错误日志记录
void UserDatabase::logError(const QString &message, const QSqlError &error)
{
    qDebug() << message << error.text();
}
