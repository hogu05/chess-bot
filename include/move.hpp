#ifndef MOVE_H
#define MOVE_H

#include <vector>

#include "bit_utils.hpp"
#include "types.hpp"

namespace move
{

constexpr int START_SQUARE_SHIFT = 0;
constexpr int TARGET_SQUARE_SHIFT = 6;
constexpr int FLAG_SHIFT = 12;
constexpr int PAWN_PROMOTION_SHIFT = 14;

constexpr int START_SQUARE_MASK = bit_utils::mask(START_SQUARE_SHIFT, 6);
constexpr int TARGET_SQUARE_MASK = bit_utils::mask(TARGET_SQUARE_SHIFT, 6);
constexpr int FLAG_MASK = bit_utils::mask(FLAG_SHIFT, 3);
constexpr int PAWN_PROMOTION_MASK = bit_utils::mask(PAWN_PROMOTION_SHIFT, 1);

constexpr int NO_FLAG = 0b000;
constexpr int EN_PASSANT_FLAG = 0b001;
constexpr int CASTLE_FLAG = 0b010;
constexpr int TWO_SPACE_PAWN_MOVE_FLAG = 0b011;
constexpr int PROMOTE_TO_KNIGHT_FLAG = 0b100;
constexpr int PROMOTE_TO_BISHOP_FLAG = 0b101;
constexpr int PROMOTE_TO_ROOK_FLAG = 0b110;
constexpr int PROMOTE_TO_QUEEN_FLAG = 0b111;

Move create_move(Square start_square, Square target_square, int flag);

Move create_move(Move move, int flag);

std::vector<Move> create_moves_from_bitboard(Square start_square, Bitboard bitboard);

Square get_start_square(Move move);

Square get_target_square(Move move);

int get_flag(Move move);

bool is_pawn_promotion(Move move);

PieceType get_pawn_promotion_piece_type(Move move);
}; // namespace move

#endif
