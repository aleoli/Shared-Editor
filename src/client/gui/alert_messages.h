#pragma once

#include <QString>

enum class Alert {
    INFO,
    WARNING,
    ERROR
};

#define INCORRECT_FIELDS QString("Check fields!")
#define WRONG_FIELDS QString("Wrong fields!")
#define LOAD_ICON_FAILED QString("Icon load failed!")
#define ICON_TOO_BIG QString("Icon too big!")
