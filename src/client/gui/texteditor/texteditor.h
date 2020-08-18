#pragma once

#include "main_window.h"
#include "alert_messages.h"

namespace Ui {
class TextEditor;
}

class TextEditor : public MainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

public slots:
    virtual void clear();

private:
    Ui::TextEditor *ui;
};
