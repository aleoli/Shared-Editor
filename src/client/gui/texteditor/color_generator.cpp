#include "color_generator.h"

#include <QString>
#include <QRandomGenerator>

#include <algorithm>

#include "exceptions.h"
#include "utils.h"

#define NUMBER_TRIES 5
#define HUE_DIFFERENCE 15

using namespace se_exceptions;

ColorGenerator::ColorGenerator() {}

QColor ColorGenerator::getColor() {
  for(int i=0; i<NUMBER_TRIES; i++) {
    auto hue = QRandomGenerator::global()->bounded(0, 360) / HUE_DIFFERENCE;
    auto sat = QRandomGenerator::global()->bounded(0, 3);
    auto light = QRandomGenerator::global()->bounded(0, 3);
    //debug("Hue: " + QString::number(hue) + " Sat: " + QString::number(sat) + " Light: " + QString::number(light));

    // Hue: 0-359 Sat: 0-255 Light: 0-255
    // Number of combinations: 360/HUE_DIFFERENCE * 3 * 3
    // If HUE_DIFFERENCE == 15 => 24*3*3 = 216 possible colors

    auto tuple = std::make_tuple(hue, sat, light);
    auto it = std::find(_colors.begin(), _colors.end(), tuple);

    if (it == _colors.end()) {
      //color not yet used
      _colors.push_back(tuple);
      return QColor::fromHsl(hue * HUE_DIFFERENCE, _saturation[sat], _lightness[light]);
    }

    if(i == NUMBER_TRIES-1) {
      // I give up, I reuse the same color..
      warn("Using the same color previously used..");
      return QColor::fromHsl(hue * HUE_DIFFERENCE, _saturation[sat], _lightness[light]);
    }
  }

  throw GuiException{"Should never reach this instruction"};
}

void ColorGenerator::reset() {
  _colors.clear();
}
