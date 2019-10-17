#pragma once

#include <QString>

QString rndString(int randomStringLength = 16);

void error(QString str);
void warn(QString str);
void debug(QString str);
void info(QString str);
