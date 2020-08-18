#pragma once

#include <QWidget>
#include <QPushButton>

#include "FSElement.h"

namespace Ui {
    class DocWidgetFolder;
}

class DocWidgetFolder: public QWidget {
Q_OBJECT

public:
    explicit DocWidgetFolder(FSElement element, QWidget *parent = nullptr);

public slots:
    virtual void clear();

signals:
    void open(int dirId);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::DocWidgetFolder *ui;

    const FSElement _element;
};
