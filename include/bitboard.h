#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>

namespace Bitboard {
    void set_square(uint64_t& bitboard, int square);
    void clear_square(uint64_t& bitboard, int square);
    int pop_square(uint64_t& bitboard);
    int get_square(uint64_t bitboard);
    bool is_set(uint64_t bitboard, int square);
    bool is_clear(uint64_t bitboard, int square);
    void clear_all(uint64_t& bitboard);
    void set_all(uint64_t& bitboard);
    uint64_t create_bitboard(int square);
    void print_bitboard(uint64_t bitboard);
};



#endif
