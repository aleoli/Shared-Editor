#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QByteArray>
#include <QBuffer>

void showImage(QImage &im) {
  QLabel *myLabel = new QLabel();
  myLabel->setPixmap(QPixmap::fromImage(im));

  myLabel->show();
}

QByteArray saveImage(QImage &im) {
  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  im.save(&buffer, "PNG");
  return ba.toBase64();
}

void prova_image(QApplication &a) {
   QImage im1;
   im1.load(":/other/malnati.png");

   auto ba = saveImage(im1);

   //std::cout << ba.toStdString() << std::endl;

   QImage im2;
   im2.loadFromData(QByteArray::fromBase64(ba), "PNG");

   //show img
   QLabel myLabel;
   myLabel.setPixmap(QPixmap::fromImage(im2));
   myLabel.show();

   std::cout << "Sono uguali: " << (im1 == im2) << std::endl;

   exit(a.exec());
}
