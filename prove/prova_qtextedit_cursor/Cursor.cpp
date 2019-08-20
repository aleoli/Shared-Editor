//
// Created by Gianluca on 20/08/2019.
//

#include "Cursor.h"
#include <QTimer>

Cursor::Cursor(QTextEdit *parent) : QLabel(parent), textEdit_(parent) {
    setStyleSheet("background-color:red;");

    cursor_ = new QTextCursor(parent->document());

    timer_ = new QTimer(this);
    QObject::connect(timer_, &QTimer::timeout, this, &Cursor::changeVisibility);
    timer_->start(500);
}

void Cursor::updateCursorPosition(int position) {
    cursor_->setPosition(position);
    auto rect = textEdit_->cursorRect(*cursor_);

    move(rect.x(), rect.y());
}

void Cursor::changeVisibility() {
    isVisible_ ? setVisible(false) : setVisible(true);
    isVisible_ = !isVisible_;
}