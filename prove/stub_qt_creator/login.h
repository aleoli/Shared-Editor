#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMovie>
#include <memory>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

    void unblock();
    void clear();

signals:
    void loginRequest(QString, QString);

public slots:
    void login();

private:
    void initLoading();
    void disable();
    void enable();
    void startLoading();
    void stopLoading();

    QLineEdit *_psw, *_username;
    QPushButton *_btn;
    QLabel *_loading;
    std::shared_ptr<QMovie> _movie;
    Ui::Login *ui;
};

#endif // LOGIN_H
