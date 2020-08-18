#pragma once

#include "alert_messages.h"
#include "main_window.h"

namespace Ui {
class Landing;
}

class Landing : public MainWindow
{
    Q_OBJECT

public:
    explicit Landing(QWidget *parent = nullptr);
    ~Landing();

public slots:
    virtual void clear();
    
private:
    Ui::Landing *ui;
};
