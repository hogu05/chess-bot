#ifndef BITBOARD_H
#define BITBOARD_H

#include <bit>

#include "types.hpp"

namespace bitboard
{
constexpr void set_square(Bitboard& bitboard, Square square)
{
    bitboard |= (1ULL << square);
}

constexpr void clear_square(Bitboard& bitboard, Square square)
{
    bitboard &= ~(1ULL << square);
}

constexpr Square get_square(Bitboard bitboard)
{
    return std::countr_zero(bitboard);
}

constexpr Square pop_square(Bitboard& bitboard)
{
    const Square square = get_square(bitboard);
    clear_square(bitboard, square);
    return square;
}

constexpr bool is_set(Bitboard bitboard, Square square)
{
    return (bitboard & (1ULL << square)) != 0;
}

constexpr bool is_clear(Bitboard bitboard, Square square)
{
    return (bitboard & (1ULL << square)) == 0;
}

constexpr void set_all(Bitboard& bitboard)
{
    bitboard = ~Bitboard{0};
}

constexpr Bitboard create_bitboard(Square square)
{
    Bitboard bitboard = 0;
    set_square(bitboard, square);
    return bitboard;
}
} // namespace bitboard

#endif
