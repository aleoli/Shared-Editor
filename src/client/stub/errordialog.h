#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorDialog;
}

class ErrorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorDialog(QWidget *parent = nullptr);
    ~ErrorDialog();

    void setErrorMsg(QString msg);
    static void showDialog(QWidget *parent, QString msg);

signals:
    void setText(QString);

private:
    Ui::ErrorDialog *ui;
};

#endif // ERRORDIALOG_H
