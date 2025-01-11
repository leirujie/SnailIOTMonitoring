#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QMap>
#include <QVariant>
#include <QList>

class UserDatabase : public QWidget
{
    Q_OBJECT
public:
    // 获取单例实例的静态函数
    static UserDatabase* getInstance();
    ~UserDatabase();

    void ensureInitialAdmin();

    // 保存用户信息，返回是否保存成功
    bool saveUserInfo(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &);

    // 删除指定用户名的用户信息，返回是否删除成功
    bool deleteUserInfo(const QString &);

    // 更新用户信息（包含角色更新）
    bool updateUserInfo(const QString &username,
                        const QString &password,
                        const QString &email,
                        const QString &phone,
                        const QString &nickname,
                        const QString &role);

    // 更新用户信息（不包含角色更新）
    bool updateUserInfo(const QString &username,
                        const QString &password,
                        const QString &email,
                        const QString &phone,
                        const QString &nickname);

    // 更新用户密码
    bool updateUserInfo(const QString &, const QString &);

    bool openDatabase();

    // 获取用户昵称，若获取失败返回空字符串
    QString getUserNickname(const QString &);

    // 根据用户名查询用户信息，若未查到返回空的QMap
    QMap<QString, QString> queryUserInfo(const QString &);

    // 查询所有用户信息，返回包含所有用户各字段信息的QMap列表，若没有用户信息则返回空列表
    QList<QMap<QString, QString>> queryAllUserInfo();

    // 检查用户是否存在（用户名、手机号、邮箱）
    bool checkUserExistence(const QString &, const QString &, const QString &);
    bool checkUsernameExists(const QString &);
    bool checkPhoneExists(const QString &);
    bool checkEmailExists(const QString &);

    //统计管理员数量
    int countAdmins();

private:
    QSqlDatabase m_db;

    // 构造函数设为私有，防止外部直接创建实例
    explicit UserDatabase(QWidget *parent = nullptr);

    // 复制构造函数和赋值运算符也设为私有，避免意外复制实例
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;

    void closeDatabase();
    void createTableIfNotExists();
    void logError(const QString &message, const QSqlError &error);
};

#endif // USERDATABASE_H
