#ifndef RECORD_H
#define RECORD_H

#include <QWidget>
#include "account.h"
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

private slots:
    void on_pushButton_3_clicked();

private:
    Ui::Record *ui;
    Account user;
signals:
    void showLoginPage();
    void rec_successful();
};

#endif // RECORD_H
