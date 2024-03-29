#pragma once

#include <QString>

enum class Alert {
    INFO,
    WARNING,
    ERROR
};

QString getAlertTitle(Alert alert);

#define GET_LINK_TITLE QString("Share link")
#define LOGIN_ERROR QString("Login error")
#define REGISTRATION_ERROR QString("Registration error")
#define EDIT_ERROR QString("Edit error")

#define WRONG_HOST QString("Address must not be empty!")
#define WRONG_PORT QString("Port must be an int between 1 and 65535!")
#define INCORRECT_FIELDS QString("Check fields!")
#define WRONG_PASSWORD QString("Password has to contain at least an Uppercase char, a Lowercase char, a number, and length are to be equal or more than 6")
#define PSW_MISMATCH QString("Password and confirm password are different!")
#define LOAD_ICON_FAILED QString("Icon load failed!")
#define ICON_TOO_BIG QString("Icon too big!")
#define SAME_PASSWORDS QString("Old password and new password are the same!")
#define NO_CHANGES QString("No changes made!")
#define INVALID_COLOR QString("Invalid color!")
#define HIGHLIGHT_ON QString("Disable user highlighting first!")
#define INVALID_SIZE QString("Invalid size!")
