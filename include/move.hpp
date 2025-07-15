#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <vector>

#include "bit_utils.hpp"
#include "types.hpp"

namespace Move
{

constexpr int START_SQUARE_SHIFT = 0;
constexpr int TARGET_SQUARE_SHIFT = 6;
constexpr int FLAG_SHIFT = 12;
constexpr int PAWN_PROMOTION_SHIFT = 14;

constexpr int START_SQUARE_MASK = BitUtils::mask(START_SQUARE_SHIFT, 6);
constexpr int TARGET_SQUARE_MASK = BitUtils::mask(TARGET_SQUARE_SHIFT, 6);
constexpr int FLAG_MASK = BitUtils::mask(FLAG_SHIFT, 3);
constexpr int PAWN_PROMOTION_MASK = BitUtils::mask(PAWN_PROMOTION_SHIFT, 1);

constexpr int NO_FLAG = 0b000;
constexpr int EN_PASSANT_FLAG = 0b001;
constexpr int CASTLE_FLAG = 0b010;
constexpr int TWO_SPACE_PAWN_MOVE_FLAG = 0b011;
constexpr int PROMOTE_TO_KNIGHT_FLAG = 0b100;
constexpr int PROMOTE_TO_BISHOP_FLAG = 0b101;
constexpr int PROMOTE_TO_ROOK_FLAG = 0b110;
constexpr int PROMOTE_TO_QUEEN_FLAG = 0b111;

Move_t create_move(Square_t start_square, Square_t target_square, int flag);

Move_t create_move(Move_t move, int flag);

std::vector<Move_t> create_moves_from_bitboard(Square_t start_square, Bitboard_t bitboard);

Square_t get_start_square(Move_t move);

Square_t get_target_square(Move_t move);

int get_flag(Move_t move);

bool is_pawn_promotion(Move_t move);

PieceType_t get_pawn_promotion_piece_type(Move_t move);

std::string get_move_notation(Move_t move);
}; // namespace Move

#endif
