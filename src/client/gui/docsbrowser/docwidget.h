#pragma once

#include <QWidget>
#include <QPushButton>

#include "FSElement.h"

namespace Ui {
    class DocWidget;
}

class DocWidget: public QWidget {
    Q_OBJECT

public:
    explicit DocWidget(FSElement element, QWidget *parent = nullptr);

public slots:
    virtual void clear();

signals:
  void open(int fileId);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::DocWidget *ui;

    const FSElement _element;
};
