#include "validator.h"

bool Validator::isValidEmail(const QString &email)
{
    QRegularExpression emailRegex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    return emailRegex.match(email).hasMatch();
}

bool Validator::isValidPhone(const QString &phone)
{
    QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    return phoneRegex.match(phone).hasMatch();
}

bool Validator::isValidUsername(const QString &username)
{
    QRegularExpression usernameRegex("^[a-zA-Z0-9]{4,20}$");
    return usernameRegex.match(username).hasMatch();
}

bool Validator::isValidPassword(const QString &password)
{
    QRegularExpression passwordRegex("^(?=.*[A-Za-z])(?=.*\\d)[A-Za-z\\d]{8,}$");
    return passwordRegex.match(password).hasMatch();
}
