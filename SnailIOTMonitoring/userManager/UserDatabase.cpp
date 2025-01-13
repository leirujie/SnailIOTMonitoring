#include "UserDatabase.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QMessageBox>
#include "validator.h"
#include "QCryptographicHash"

// 静态成员变量，用于保存单例实例指针
UserDatabase* UserDatabase::getInstance()
{
    static UserDatabase instance;
    return &instance;
}

UserDatabase::UserDatabase(QWidget *parent) : QWidget(parent)
{
    if (!openDatabase()) {
        QMessageBox::critical(this, "数据库错误", "无法打开数据库连接！");
        LogManager::instance().logMessage(LogManager::ERROR, "exception", "用户数据库连接失败");
        return;
    }
    createTableIfNotExists();//创建表
    ensureInitialAdmin();//添加一个默认管理员账号
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
    QSqlDatabase::removeDatabase("my_connection_name"); // 移除连接
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

//默认管理员
void UserDatabase::ensureInitialAdmin()
{
    // 检查是否存在管理员账号
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE role = :role");
    query.bindValue(":role", "admin");
    if (!query.exec()) {
        logError("Error checking admin existence:", query.lastError());
        return;
    }
    if (query.next()) {
        int adminCount = query.value(0).toInt();
        if (adminCount == 0) {
            // 如果不存在管理员，创建一个默认的管理员账号
            QString defaultAdminUsername = "admin";
            QString defaultAdminPassword = "admin123"; // 请务必在首次登录后更改此密码

            QByteArray hash = QCryptographicHash::hash(defaultAdminPassword.toUtf8(), QCryptographicHash::Sha256);
            QString encryptedPassword = hash.toHex();  // 获取加密后的密码
            QString defaultAdminEmail = "admin123@qq.com";
            QString defaultAdminPhone = "13195849795";
            QString defaultAdminNickname = "Admin";
            QString role = "admin";

            bool saveSuccess = saveUserInfo(defaultAdminUsername, encryptedPassword, defaultAdminEmail, defaultAdminPhone, defaultAdminNickname, role);
            if (saveSuccess) {
                qDebug() << "初始管理员账号已创建。用户名：" << defaultAdminUsername << " 密码：" << defaultAdminPassword;
                QMessageBox::information(this, "初始化成功", "默认管理员账号已创建。\n用户名: admin\n密码: admin123\n请登录后立即更改密码。");
            } else {
                qDebug() << "创建初始管理员账号失败。";
                QMessageBox::warning(this, "初始化失败", "无法创建默认管理员账号，请联系管理员。");
                LogManager::instance().logMessage(LogManager::ERROR, "system", "创建默认管理员账号失败");
            }
        }
    }
}


bool UserDatabase::checkUsernameExists(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        logError("Error checking username existence:", query.lastError());
        return true;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool UserDatabase::checkPhoneExists(const QString &phone)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE phone = :phone");
    query.bindValue(":phone", phone);
    if (!query.exec()) {
        logError("Error checking phone existence:", query.lastError());
        return true;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool UserDatabase::checkEmailExists(const QString &email)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE email = :email");
    query.bindValue(":email", email);
    if (!query.exec()) {
        logError("Error checking email existence:", query.lastError());
        return true;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool UserDatabase::checkUserExistence(const QString &username, const QString &phone, const QString &email)
{
    QSqlQuery query(m_db);
    QString queryString = "SELECT COUNT(*) FROM users WHERE ";
    QList<QString> conditions;

    if (!username.isEmpty()) {
        conditions.append("username = :username");
    }
    if (!phone.isEmpty()) {
        conditions.append("phone = :phone");
    }
    if (!email.isEmpty()) {
        conditions.append("email = :email");
    }

    if (conditions.isEmpty()) {
        // 如果所有参数都为空，无法进行有效查询
        logError("No parameters provided for user existence check.", QSqlError());
        return false;
    }

    queryString += conditions.join(" OR ");
    query.prepare(queryString);

    if (!username.isEmpty()) {
        query.bindValue(":username", username);
    }
    if (!phone.isEmpty()) {
        query.bindValue(":phone", phone);
    }
    if (!email.isEmpty()) {
        query.bindValue(":email", email);
    }

    if (!query.exec()) {
        logError("Error checking user existence:", query.lastError());
        return true; // 假设查询失败时返回 true，以防止重复注册
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

// 统计管理员数量
int UserDatabase::countAdmins()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE role = :role");
    query.bindValue(":role", "admin");
    if (!query.exec()) {
        logError("Error counting admins:", query.lastError());
        return 0;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

// 保存用户信息
bool UserDatabase::saveUserInfo(const QString &username, const QString &password, const QString &email, const QString &phone, const QString &nickname, const QString &role)
{
    // 使用 Validator 类进行格式验证
    if (!Validator::isValidUsername(username)) {
        qDebug() << "Invalid username format.";
        return false;
    }

    if (!Validator::isValidPassword(password)) {
        qDebug() << "Invalid password format.";
        return false;
    }

    if (!Validator::isValidEmail(email)) {
        qDebug() << "Invalid email format.";
        return false;
    }

    if (!Validator::isValidPhone(phone)) {
        qDebug() << "Invalid phone format.";
        return false;
    }

    // 角色验证
    if (role != "user" && role != "admin") {
        qDebug() << "Invalid role.";
        return false;
    }

    QSqlQuery query(m_db);

    query.prepare("INSERT INTO users (username, password, email, phone, nickname, role) "
                  "VALUES (:username, :password, :email, :phone, :nickname, :role)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":phone", phone);
    query.bindValue(":nickname", nickname);
    query.bindValue(":role", role);

    if (!query.exec()) {
        logError("Error saving user info:", query.lastError());
        return false;
    } else {
        qDebug() << "User info saved successfully!";
        return true;
    }
}

// 删除指定用户名的用户信息
bool UserDatabase::deleteUserInfo(const QString &username)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        logError("Error deleting user:", query.lastError());
        return false;
    }
    return true;
}

// 更新用户密码
bool UserDatabase::updateUserInfo(const QString &username, const QString &newPassword)
{
    // 使用 Validator 类进行格式验证
    if (!Validator::isValidUsername(username)) {
        qDebug() << "Invalid username format.";
        return false;
    }

    if (!Validator::isValidPassword(newPassword)) {
        qDebug() << "Invalid password format.";
        return false;
    }

    // 构造 SQL 查询进行更新密码
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET password = :password WHERE username = :username");
    query.bindValue(":username", username);
    query.bindValue(":password", newPassword);

    if (!query.exec()) {
        qDebug() << "更新密码失败，错误信息：" << query.lastError();
        return false;
    }

    // 检查是否有行被更新
    return query.numRowsAffected() > 0;
}


// 更新用户信息（包含角色更新）
bool UserDatabase::updateUserInfo(const QString &username,
                                  const QString &password,
                                  const QString &email,
                                  const QString &phone,
                                  const QString &nickname,
                                  const QString &role)
{
    // 使用 Validator 类进行格式验证
    if (!Validator::isValidUsername(username)) {
        qDebug() << "Invalid username format.";
        return false;
    }

    if (!password.isEmpty() && !Validator::isValidPassword(password)) {
        qDebug() << "Invalid password format.";
        return false;
    }

    if (!email.isEmpty() && !Validator::isValidEmail(email)) {
        qDebug() << "Invalid email format.";
        return false;
    }

    if (!phone.isEmpty() && !Validator::isValidPhone(phone)) {
        qDebug() << "Invalid phone format.";
        return false;
    }

    // 角色验证
    if (role != "user" && role != "admin") {
        qDebug() << "Invalid role.";
        return false;
    }

    // 验证手机号是否已经存在
    if (!phone.isEmpty()) {
        QSqlQuery checkPhoneQuery(m_db);
        checkPhoneQuery.prepare("SELECT COUNT(*) FROM users WHERE phone = :phone AND username != :username");
        checkPhoneQuery.bindValue(":phone", phone);
        checkPhoneQuery.bindValue(":username", username);
        if (!checkPhoneQuery.exec()) {
            logError("Error checking phone existence:", checkPhoneQuery.lastError());
            return false;
        }
        checkPhoneQuery.next();
        if (checkPhoneQuery.value(0).toInt() > 0) {
            qDebug() << "Phone number already exists.";
            return false;
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
        return false;
    } else {
        qDebug() << "用户信息更新成功。";
        return true;
    }
}


// 更新用户信息（不包含角色更新）
bool UserDatabase::updateUserInfo(const QString &username,
                                  const QString &password,
                                  const QString &email,
                                  const QString &phone,
                                  const QString &nickname)
{
    // 使用 Validator 类进行格式验证
    if (!Validator::isValidUsername(username)) {
        qDebug() << "Invalid username format.";
        return false;
    }

    if (!password.isEmpty() && !Validator::isValidPassword(password)) {
        qDebug() << "Invalid password format.";
        return false;
    }

    if (!email.isEmpty() && !Validator::isValidEmail(email)) {
        qDebug() << "Invalid email format.";
        return false;
    }

    if (!phone.isEmpty() && !Validator::isValidPhone(phone)) {
        qDebug() << "Invalid phone format.";
        return false;
    }

    // 验证手机号是否已经存在
    if (!phone.isEmpty()) {
        QSqlQuery checkPhoneQuery(m_db);
        checkPhoneQuery.prepare("SELECT COUNT(*) FROM users WHERE phone = :phone AND username != :username");
        checkPhoneQuery.bindValue(":phone", phone);
        checkPhoneQuery.bindValue(":username", username);
        if (!checkPhoneQuery.exec()) {
            logError("Error checking phone existence:", checkPhoneQuery.lastError());
            return false;
        }
        checkPhoneQuery.next();
        if (checkPhoneQuery.value(0).toInt() > 0) {
            qDebug() << "Phone number already exists.";
            return false;
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
        return false;
    } else {
        qDebug() << "用户信息更新成功。";
        return true;
    }
}


QString UserDatabase::getUserNickname(const QString &username)
{
    if (username.isEmpty()) {
        qDebug() << "用户名不能为空。";
        return "";
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT nickname FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "获取用户昵称失败，错误信息：" << query.lastError().text();
        return "";
    }

    if (query.next()) {
        return query.value("nickname").toString();
    } else {
        qDebug() << "未找到对应的用户昵称。";
        return "";
    }
}

// 根据用户名查询用户信息
QMap<QString, QString> UserDatabase::queryUserInfo(const QString &username)
{
    if (!m_db.isOpen()) {
        if (!openDatabase()) {
            logError("Database not open in queryUserInfo:", m_db.lastError());
            return QMap<QString, QString>();
        }
    }

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
