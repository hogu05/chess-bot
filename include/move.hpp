#ifndef MOVE_H
#define MOVE_H

#include "bit_utils.hpp"
#include "bitboard.hpp"
#include "move_list.hpp"
#include "piece.hpp"
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

constexpr Move NONE_MOVE = 0;

constexpr Move create_move(Square start_square, Square target_square, int flag)
{
    return (start_square << START_SQUARE_SHIFT) | (target_square << TARGET_SQUARE_SHIFT) |
           (flag << FLAG_SHIFT);
}

constexpr void add_moves_from_bitboard(MoveList& moves, Square start_square, Bitboard targets)
{
    while (targets != 0)
    {
        moves.push_back(create_move(start_square, bitboard::pop_square(targets), NO_FLAG));
    }
}

constexpr Square get_start_square(Move move)
{
    return (move & START_SQUARE_MASK) >> START_SQUARE_SHIFT;
}

constexpr Square get_target_square(Move move)
{
    return (move & TARGET_SQUARE_MASK) >> TARGET_SQUARE_SHIFT;
}

constexpr int get_flag(Move move)
{
    return (move & FLAG_MASK) >> FLAG_SHIFT;
}

constexpr bool is_pawn_promotion(Move move)
{
    return (move & PAWN_PROMOTION_MASK) != 0;
}

constexpr PieceType get_pawn_promotion_piece_type(Move move)
{
    switch (get_flag(move))
    {
    case PROMOTE_TO_KNIGHT_FLAG:
        return piece::KNIGHT;
    case PROMOTE_TO_BISHOP_FLAG:
        return piece::BISHOP;
    case PROMOTE_TO_ROOK_FLAG:
        return piece::ROOK;
    case PROMOTE_TO_QUEEN_FLAG:
        return piece::QUEEN;
    default:
        return piece::NONE;
    }
}
} // namespace move

#endif
