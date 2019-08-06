#include <iostream>
#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrinter>
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    doc.setHtml("<h1>Hello, World!</h1>\n<p>Lorem ipsum dolor sit amet, consectitur adipisci elit.</p>");
    doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
    doc.print(&printer);
}
