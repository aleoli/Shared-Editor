#ifndef LANDING_H
#define LANDING_H

#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <memory>

namespace Ui {
class Landing;
}

class Landing : public QDialog
{
    Q_OBJECT

public:
    explicit Landing(QWidget *parent = nullptr);
    ~Landing();

private:
    Ui::Landing *ui;

    QLabel *_loading;
    std::shared_ptr<QMovie> _movie;
};

#endif // LANDING_H
