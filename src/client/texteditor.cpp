#include "texteditor.h"
#include "ui_texteditor.h"
#include <QPrinter>
#include <QPrintDialog>

TextEditor::TextEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextEditor)
{
    ui->setupUi(this);
    ui->comboBox->addItem("Klajdi");
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::on_pushButton_2_clicked()
{
    QPrinter printer;
    printer.setPrinterName("HP-Deskjet-F300 Series");
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Rejected) return;
    ui->plainTextEdit->print(&printer);
}
