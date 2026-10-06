#ifndef SQUARE_H
#define SQUARE_H

#include <cstdint>

#include "types.hpp"

namespace square
{
constexpr int FILES = 8;
constexpr int RANKS = 8;
constexpr int TOTAL_SQUARES = FILES * RANKS;

// clang-format off
enum : std::uint8_t {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};
// clang-format on

constexpr int get_file(Square square)
{
    return square & (FILES - 1);
}

constexpr int get_rank(Square square)
{
    return square >> 3;
}

constexpr Square create_square(int file, int rank)
{
    return (rank << 3) | file;
}

constexpr bool is_valid(Square square)
{
    return square >= 0 && square < TOTAL_SQUARES;
}
} // namespace square

#endif
