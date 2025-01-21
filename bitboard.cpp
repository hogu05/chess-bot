#include "bitboard.h"
#include <array>
#include <iostream>
#include <bitset>

void Bitboard::set_square(uint64_t& bitboard, int square) {
    bitboard |= (1ULL << square);
};

void Bitboard::clear_square(uint64_t& bitboard, int square) {
    bitboard &= ~(1ULL << square);
};

int Bitboard::pop_square(uint64_t& bitboard) {
    int square = __builtin_ctzll(bitboard);
    clear_square(bitboard, square);
    return square;
};


void Bitboard::print_bitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; rank--) {
        uint64_t line = (bitboard >> rank * 8) & 0b11111111;
        std::bitset<8> bits(line);
        for (int bit = 0; bit < 8; bit++) {
            std::cout << bits[bit] << " ";
        }
        std::cout << std::endl;
    }
};
