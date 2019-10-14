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
  im.save(&buffer, "JPG");
  return ba;
}

void prova_image(QApplication &a) {
   QImage im1;
   im1.load(":/other/malnati.jpg");

   auto ba = saveImage(im1);

   QImage im2;
   im2.loadFromData(ba, "JPG");

   showImage(im2);

   std::cout << "Sono uguali: " << (im1 == im2) << std::endl;

   exit(a.exec());
}
