#pragma once

#include <QMainWindow>

#include "alert_messages.h"

namespace Ui {
class TextEditor;
}

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private slots:

signals:
  void alert(Alert type, QString what);

private:
    Ui::TextEditor *ui;
};
