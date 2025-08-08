#ifndef PRECOMPUTATIONS_H
#define PRECOMPUTATIONS_H

#include <array>

#include "board.hpp"
#include "types.hpp"

namespace Precomputations
{
void init_precomputations();
int get_squares_to_edge(Square_t square, Direction_t direction);

extern std::array<std::array<Bitboard_t, Board::TOTAL_SQUARES>, Board::COLORS> pawn_attacks;
extern std::array<Bitboard_t, Board::TOTAL_SQUARES> knight_moves;
extern std::array<Bitboard_t, Board::TOTAL_SQUARES> king_moves;
}; // namespace Precomputations

#endif
