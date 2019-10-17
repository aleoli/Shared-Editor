#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QByteArray>
#include <QBuffer>
#include <string>

void showImage(QImage &im) {
  QLabel *myLabel = new QLabel();
  myLabel->setPixmap(QPixmap::fromImage(im));

  myLabel->show();
}

std::string saveImage(QImage &im) {
  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  im.save(&buffer, "PNG");
  return ba.toBase64().toStdString();
}

void prova_image(QApplication &a) {
  std::cout << "Test serializzazione QImage" << std::endl;

  QImage im1;
  im1.load(":/other/malnati.png");

  auto ba = saveImage(im1);

  //std::cout << ba << std::endl;

  QImage im2;
  im2.loadFromData(QByteArray::fromBase64(QByteArray::fromStdString(ba)), "PNG");

  //show img
  QLabel myLabel;
  myLabel.setPixmap(QPixmap::fromImage(im2));
  myLabel.show();

  if(im1 != im2) {
   throw TestException{"test copia QImage fallito"};
  }

  std::cout << "Test passato" << std::endl;

  exit(a.exec());
}
