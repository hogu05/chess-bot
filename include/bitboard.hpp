#ifndef BITBOARD_H
#define BITBOARD_H

#include "types.hpp"

namespace Bitboard
{
    void set_square(Bitboard_t &bitboard, Square_t square);

    void clear_square(Bitboard_t &bitboard, Square_t square);

    Square_t pop_square(Bitboard_t &bitboard);

    Square_t get_square(Bitboard_t bitboard);

    bool is_set(Bitboard_t bitboard, Square_t square);

    bool is_clear(Bitboard_t bitboard, Square_t square);

    void clear_all(Bitboard_t &bitboard);

    void set_all(Bitboard_t &bitboard);

    Bitboard_t create_bitboard(Square_t square);

    void print_bitboard(Bitboard_t bitboard);
}


#endif
