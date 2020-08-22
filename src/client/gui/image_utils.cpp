#include "image_utils.h"

#include <QImage>
#include <QByteArray>
#include <QString>
#include <QBuffer>
#include <QDir>
#include <QStringList>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QPainterPath>
#include <QFileInfo>
#include <QWindow>

#include <vector>

#include "exceptions.h"
#include "utils.h"

using namespace se_exceptions;

QIcon image_utils::loadImage(const QString &path) {
  auto image = QIcon{path};

  if(image.isNull()) {
   throw ImageException{"Image not found!"};
  }

  return image;
}

QIcon image_utils::decodeImage(const QString &encoded) {
  auto pixmap = QPixmap();
  pixmap.loadFromData(QByteArray::fromBase64(encoded.toUtf8()));
  return QIcon(pixmap);
}

QString image_utils::encodeImage(const QIcon &icon) {
  QByteArray ba;
  QBuffer buffer(&ba);
  buffer.open(QIODevice::WriteOnly);
  icon.pixmap(300,300).toImage().save(&buffer, "PNG");

  return QString::fromUtf8(ba.toBase64());
}

std::vector<QIcon> image_utils::getAllProfilePics() {
  std::vector<QIcon> images;

  QDir directory(PROFILE_PICS_PATH);
  QStringList names = directory.entryList(QStringList() << "*.png" << "*.PNG",QDir::Files);
  foreach(QString filename, names) {
    images.push_back(loadImage(PROFILE_PICS_PATH + filename));
  }

  return images;
}

QIcon image_utils::loadRoundedImage(const QString &filename) {
  checkIcon(filename);

  QImage image = QImage(filename);

  if(image.isNull()) {
   throw ImageException{"Could not load image!"};
  }

  image = image.convertToFormat(QImage::Format_ARGB32);
  int size = qMin(image.width(), image.height());
  auto rect = QRect(
          (image.width() - size) / 2,
          (image.height() - size) / 2,
          size,
          size
  );
  image = image.copy(rect);

  QImage out_img = QImage(size, size, QImage::Format_ARGB32);
  out_img.fill(Qt::transparent);

  auto brush = QBrush(image);
  QPainter painter (&out_img);
  painter.setBrush(brush);
  painter.setPen(Qt::NoPen);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.drawEllipse(0, 0, size, size);
  painter.end();

  auto pr = QWindow().devicePixelRatio();
  auto pm = QPixmap::fromImage(out_img);
  pm.setDevicePixelRatio(pr);
  size *= pr;
  pm = pm.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  return QIcon(pm);
}

void image_utils::checkIcon(const QString &filename) {
  auto file = QFileInfo{filename};

  if(!file.exists()) {
    throw ImageException{"Image not found!"};

  }

  if(file.size() > MAX_SIZE) {
    throw ImageException{"Image is too big!"};
  }
}
