#ifndef COLOR_H
#define COLOR_H

#include "types.hpp"

namespace color
{
constexpr Color WHITE = 0;
constexpr Color BLACK = 1;
constexpr int COLORS = 2;

constexpr Color get_other_color(Color color)
{
    return color == WHITE ? BLACK : WHITE;
}
} // namespace color

#endif
