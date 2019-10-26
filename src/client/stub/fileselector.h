#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <memory>
#include <QMovie>
#include <QPushButton>

namespace Ui {
class FileSelector;
}

class FileSelector : public QDialog
{
    Q_OBJECT

public:
    explicit FileSelector(QWidget *parent = nullptr);
    ~FileSelector();

    void unblock();
    void clear();

public slots:
    void openFile();
    void newFile();

signals:
    void openFileRequest(int);
    void newFileRequest();

private:
    void initLoading();
    void disable();
    void enable();
    void startLoading();
    void stopLoading();

    QLineEdit *_id;
    QPushButton *_btnOpen, *_btnNew;
    Ui::FileSelector *ui;

    QLabel *_loading;
    std::shared_ptr<QMovie> _movie;
};

#endif // FILESELECTOR_H
