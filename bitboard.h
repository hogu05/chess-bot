#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>

class Bitboard {
public:
    static void set_square(uint64_t& bitboard, int square);
    static void clear_square(uint64_t& bitboard, int square);
    static int pop_square(uint64_t& bitboard);
    static int get_square(uint64_t bitboard);
    static bool is_set(uint64_t bitboard, int square);
    static bool is_clear(uint64_t bitboard, int square);
    static void clear_all(uint64_t& bitboard);
    static void set_all(uint64_t& bitboard);
    static uint64_t create_bitboard(int square);

    static void print_bitboard(uint64_t bitboard);
};



#endif
