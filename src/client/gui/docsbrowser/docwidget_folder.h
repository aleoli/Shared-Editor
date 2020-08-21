#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include "FSElement.h"

namespace Ui {
    class DocWidgetFolder;
}

class DocWidgetFolder: public QWidget {
Q_OBJECT

public:
    explicit DocWidgetFolder(FSElement element, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent* e) override;

public slots:
    virtual void clear();

signals:
    void open(int dirId);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::DocWidgetFolder *ui;

    QLabel *_nameLabel, *_dateLabel;
    QPushButton *_optionsBtn;

    const FSElement _element;
};
