#include "bitboard.hpp"

#include <bitset>
#include <iostream>

#include "types.hpp"
namespace Bitboard
{
void set_square(Bitboard_t& bitboard, Square_t square)
{
    bitboard |= (1ULL << square);
}

void clear_square(Bitboard_t& bitboard, Square_t square)
{
    bitboard &= ~(1ULL << square);
}

Square_t pop_square(Bitboard_t& bitboard)
{
    Square_t square = __builtin_ctzll(bitboard);
    clear_square(bitboard, square);
    return square;
}

Square_t get_square(Bitboard_t bitboard)
{
    Square_t square = __builtin_ctzll(bitboard);
    return square;
}

bool is_set(Bitboard_t bitboard, Square_t square)
{
    return (bitboard & (1ULL << square)) != 0;
}

bool is_clear(Bitboard_t bitboard, Square_t square)
{
    return (bitboard & (1ULL << square)) == 0;
}

void clear_all(Bitboard_t& bitboard)
{
    bitboard = 0;
}

void set_all(Bitboard_t& bitboard)
{
    bitboard = 0xFFFFFFFFFFFFFFFF;
}

Bitboard_t create_bitboard(Square_t square)
{
    Bitboard_t bitboard = 0;
    set_square(bitboard, square);
    return bitboard;
}

void print_bitboard(Bitboard_t bitboard)
{
    for (int rank = 7; rank >= 0; rank--)
    {
        Bitboard_t line = (bitboard >> rank * 8) & 0b11111111;
        std::bitset<8> bits(line);
        for (int bit = 0; bit < 8; bit++)
        {
            std::cout << bits[bit] << " ";
        }
        std::cout << std::endl;
    }
}
} // namespace Bitboard
