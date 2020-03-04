#ifndef RECORD_H
#define RECORD_H

#include <QWidget>
#include <QString>
#include "string.h"

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


    string getUsername() const;
    void setUsername(const string &value);

    string getPassword() const;
    void setPassword(const string &value);

    string getRepeat_password() const;
    void setRepeat_password(const string &value);

    string getNickname() const;
    void setNickname(const string &value);

    string getRepeat_username() const;
    void setRepeat_username(const string &value);

private slots:

    void on_pushButton_signUp_clicked();

    void on_pushButton_logIn_clicked();

    //void on_lineEdit_nickname_textEdited(const QString &arg1);

    void on_checkBox_2_toggled(bool checked);

private:
    Ui::Record *ui;
    string username;
    //string repeat_username;*/
    string password;
    string repeat_password;
    string nickname;
    //Manager *m;
signals:
    void showLoginPage();
    void record_try(string nickname, string username, string password, string repeat_password);
};

#endif // RECORD_H
