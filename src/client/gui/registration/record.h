#ifndef RECORD_H
#define RECORD_H

#include <QWidget>
//#include <QString>
//#include "QString.h"

using namespace std;

namespace Ui {
class Record;
}

class Record : public QWidget
{
    Q_OBJECT

public:
    explicit Record(QWidget *parent = nullptr);
    ~Record();
    //void setManagerClass(Manager *m){this->m=m;}


    QString getUsername() const;
    void setUsername(const QString &value);

    QString getPassword() const;
    void setPassword(const QString &value);

    QString getRepeat_password() const;
    void setRepeat_password(const QString &value);

    QString getNickname() const;
    void setNickname(const QString &value);

    QString getRepeat_username() const;
    void setRepeat_username(const QString &value);

private slots:

    void on_pushButton_signUp_clicked();

    void on_pushButton_logIn_clicked();

    //void on_lineEdit_nickname_textEdited(const QQString &arg1);

    void on_checkBox_2_toggled(bool checked);

private:
    Ui::Record *ui;
    QString username;
    //QString repeat_username;*/
    QString password;
    QString repeat_password;
    QString nickname;
    //Manager *m;
signals:
    void showLoginPage();
    void record_try(QString nickname, QString username, QString password, QString repeat_password);
};

#endif // RECORD_H
