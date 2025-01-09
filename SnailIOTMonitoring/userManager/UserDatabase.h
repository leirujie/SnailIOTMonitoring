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
    static UserDatabase* getInstance(QWidget *parent = nullptr);
    ~UserDatabase();

    // 保存用户信息
    void saveUserInfo(const QString &username, const QString &password, const QString &email, const QString &phone, const QString &nickname, const QString &role);

    // 删除指定用户名的用户信息
    void deleteUserInfo(const QString &username);

    // 更新用户信息
    void updateUserInfo(const QString &username,
                        const QString &password,
                        const QString &email,
                        const QString &phone,
                        const QString &nickname,
                        const QString &role);

    void updateUserInfo(const QString &username,
                                      const QString &password,
                                      const QString &email,
                                      const QString &phone,
                                      const QString &nickname);

    bool updateUserInfo(const QString& , const QString& );

    // 根据用户名查询用户信息
    QMap<QString, QString> queryUserInfo(const QString &username);
    // 查询所有用户信息，返回包含所有用户各字段信息的QMap列表
    QList<QMap<QString, QString>> queryAllUserInfo();

    bool checkUserExistence(const QString &username, const QString &phone, const QString &email);

private:
    QSqlDatabase m_db;

    // 构造函数设为私有，防止外部直接创建实例
    explicit UserDatabase(QWidget *parent = nullptr);

    // 复制构造函数和赋值运算符也设为私有，避免意外复制实例
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;

    bool openDatabase();
    void closeDatabase();
    void createTableIfNotExists();
    void logError(const QString &message, const QSqlError &error);
};

#endif // USERDATABASE_H
