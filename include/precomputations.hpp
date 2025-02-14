#ifndef PRECOMPUTATIONS_H
#define PRECOMPUTATIONS_H

#include <array>
#include <cstdint>
#include "board.hpp"

class Precomputations {
public:
    static void init();
    static int get_squares_to_edge(int square, int direction);
    static std::array<std::array<uint64_t, Board::TOTAL_SQUARES>, 2> pawn_attacks;
    static std::array<uint64_t, Board::TOTAL_SQUARES> knight_moves;
    static std::array<uint64_t, Board::TOTAL_SQUARES> king_moves;
private:
    static void calculate_squares_to_edge();
    static void calculate_pawn_attacks();
    static void calculate_knight_moves();
    static void calculate_king_moves();
    static std::array<std::array<int, 8>, Board::TOTAL_SQUARES> squares_to_edge;
};



#endif
