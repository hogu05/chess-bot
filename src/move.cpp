#include "move.hpp"

#include "bitboard.hpp"
#include "piece.hpp"

namespace move
{
Move create_move(Square start_square, Square target_square, int flag)
{
    Move move = 0;
    move = move | (start_square << START_SQUARE_SHIFT);
    move = move | (target_square << TARGET_SQUARE_SHIFT);
    move = move | (flag << FLAG_SHIFT);
    return move;
}

Move create_move(Move move, int flag)
{
    move &= ~FLAG_MASK;
    move |= (flag << FLAG_SHIFT);
    return move;
}

std::vector<Move> create_moves_from_bitboard(Square start_square, Bitboard bitboard)
{
    std::vector<Move> moves;
    while (bitboard != 0)
    {
        Square target_square = bitboard::pop_square(bitboard);
        moves.push_back(create_move(start_square, target_square, NO_FLAG));
    }
    return moves;
}

Square get_start_square(Move move)
{
    return (move & START_SQUARE_MASK) >> START_SQUARE_SHIFT;
}

Square get_target_square(Move move)
{
    return (move & TARGET_SQUARE_MASK) >> TARGET_SQUARE_SHIFT;
}

int get_flag(Move move)
{
    return (move & FLAG_MASK) >> FLAG_SHIFT;
}

bool is_pawn_promotion(Move move)
{
    return (move & PAWN_PROMOTION_MASK) != 0;
}

PieceType get_pawn_promotion_piece_type(Move move)
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
