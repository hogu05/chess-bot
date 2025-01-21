#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>

class Bitboard {
public:
    static void set_square(uint64_t& bitboard, int square);
    static void clear_square(uint64_t& bitboard, int square);
    static int pop_square(uint64_t& bitboard);

    static void print_bitboard(uint64_t bitboard);
};



#endif
