//
// Created by aleoli on 07/08/19.
//

#include "TextManager.h"
#include <iostream>

#include "TextEdit.h"

TextManager::TextManager(QObject *parent): QObject(parent) {
    this->pos = 0;
    this->str = "";
    this->html_tags.emplace_back("<br>");
    this->html_tags.emplace_back("<em>");
    this->html_tags.emplace_back("</em>");
}

void TextManager::addStr(QString str) {
    TextEdit *te = qobject_cast<TextEdit*>(this->parent());
    te->setRemoteInsert(true);
    // va poi messo nel punto giusto della stringa, è solo per capire come funziona
    this->str.insert(0, str);
    emit this->str_loaded(str, 0);
    te->setRemoteInsert(false);
    std::cout << this->str.toStdString() << std::endl;
}

void TextManager::addChr(QChar chr) {
    // genera symbol e invia al server
    if(chr == '\n') {
        // invia un <br> al server
    }
    this->str.insert(pos-1, chr);
    std::cout << this->str.toStdString() << std::endl;
}

QString TextManager::loadFromServer() {
    auto tmp = "<em>ciao</em><br>ciao<br>";
    this->str = QString(tmp);
    this->str.replace("<br>", "\n");
    for(const auto &e: this->html_tags) {
        this->str.replace(e, "");
    }
    return tmp;
}

void TextManager::setPos(int pos) {
    this->pos = pos;
}
