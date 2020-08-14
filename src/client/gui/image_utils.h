#pragma once

#include <QImage>
#include <QString>
#include <vector>

#define PROFILE_PICS_PATH QString(":res/profile-pics/")
#define DEFAULT_ICON PROFILE_PICS_PATH + QString("default.png")

namespace image_utils {
  QIcon loadImage(const QString &path);
  QIcon decodeImage(const QString &encoded);
  QString encodeImage(const QIcon &image);
  std::vector<QIcon> getAllProfilePics();
  QIcon loadRoundedImage(const QString &filename);
}
