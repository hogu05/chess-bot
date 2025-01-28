#include "bitboard.h"
#include <array>
#include <iostream>
#include <bitset>

void Bitboard::set_square(uint64_t& bitboard, int square) {
    bitboard |= (1ULL << square);
}

void Bitboard::clear_square(uint64_t& bitboard, int square) {
    bitboard &= ~(1ULL << square);
}

int Bitboard::pop_square(uint64_t& bitboard) {
    int square = __builtin_ctzll(bitboard);
    clear_square(bitboard, square);
    return square;
}

int Bitboard::get_square(uint64_t bitboard) {
    int square = __builtin_ctzll(bitboard);
    return square;
}

bool Bitboard::is_set(uint64_t bitboard, int square) {
    return (bitboard & (1ULL << square)) != 0;
}

bool Bitboard::is_clear(uint64_t bitboard, int square) {
    return (bitboard & (1ULL << square)) == 0;
}

void Bitboard::clear_all(uint64_t &bitboard) {
    bitboard = 0;
}

void Bitboard::set_all(uint64_t &bitboard) {
    bitboard = 0xFFFFFFFFFFFFFFFF;
}

uint64_t Bitboard::create_bitboard(int square) {
    uint64_t bitboard = 0;
    set_square(bitboard, square);
    return bitboard;
}

void Bitboard::print_bitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; rank--) {
        uint64_t line = (bitboard >> rank * 8) & 0b11111111;
        std::bitset<8> bits(line);
        for (int bit = 0; bit < 8; bit++) {
            std::cout << bits[bit] << " ";
        }
        std::cout << std::endl;
    }
}
