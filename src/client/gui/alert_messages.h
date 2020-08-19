#pragma once

#include <QString>

enum class Alert {
    INFO,
    WARNING,
    ERROR
};

#define INCORRECT_FIELDS QString("Check fields!")
#define WRONG_PASSWORD QString("Password has to contain at least an Uppercase char, a Lowercase char, a number, and length are to be equal or more than 6")
#define PSW_MISMATCH QString("Password and confirm password are different!")
#define LOAD_ICON_FAILED QString("Icon load failed!")
#define ICON_TOO_BIG QString("Icon too big!")
#define SAME_PASSWORDS QString("Old password and new password are the same!")
#define NO_CHANGES QString("No changes made!")
