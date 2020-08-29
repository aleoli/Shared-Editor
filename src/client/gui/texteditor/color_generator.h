#pragma once

#include <QColor>

#include <vector>
#include <tuple>
#include <array>

class ColorGenerator {
public:
  ColorGenerator();

  QColor getColor();
  void reset();

private:
  std::vector<std::tuple<int,int,int>> _colors;
  std::array<int,3> _saturation{96, 176, 255};
  std::array<int,3> _lightness{64, 96, 128};
};
