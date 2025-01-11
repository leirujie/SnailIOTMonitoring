#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>
#include <QRegularExpression>

class Validator
{
public:
    // 验证邮箱格式是否合法
    static bool isValidEmail(const QString &email);

    // 验证手机号码格式是否合法（中国大陆手机号）
    static bool isValidPhone(const QString &phone);

    // 验证用户名格式是否合法（4-20位字母和数字）
    static bool isValidUsername(const QString &username);

    // 验证密码格式是否合法（至少8位，包含字母和数字）
    static bool isValidPassword(const QString &password);
};

#endif // VALIDATOR_H
