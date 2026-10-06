#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <array>

#include "board.hpp"
#include "piece.hpp"
#include "types.hpp"

namespace evaluator
{
constexpr std::array<int, piece::PIECE_TYPE_COUNT + 1> PIECE_VALUES = {0,   100, 300, 300,
                                                                       500, 900, 0};

constexpr int get_piece_value(PieceType piece_type)
{
    return PIECE_VALUES[piece_type];
}

int evaluate(const Board& board);
} // namespace evaluator

#endif
