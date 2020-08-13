#pragma once

#include <QString>

enum class Alert {
    INFO,
    WARNING,
    ERROR
};

#define INCORRECT_FIELDS QString("Check fields!")
#define WRONG_FIELDS QString("Wrong fields!")
