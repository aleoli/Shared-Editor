#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>
#include "document.h"

namespace Ui {
class TextEditor;
}

class TextEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private:
    Ui::TextEditor *ui;
    Document *doc;
signals:
    void showLoginPage();
    void showDocsBrowserPage();
private slots:
    void on_pushButton_2_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_comboBox_activated(const QString &arg1);
};

#endif // TEXTEDITOR_H
