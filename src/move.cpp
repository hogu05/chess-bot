#include "move.hpp"

#include "bitboard.hpp"
#include "board.hpp"
#include "move_generator.hpp"
#include "piece.hpp"

namespace Move
{
Move_t create_move(Square_t start_square, Square_t target_square, int flag)
{
    Move_t move = 0;
    move = move | (start_square << START_SQUARE_SHIFT);
    move = move | (target_square << TARGET_SQUARE_SHIFT);
    move = move | (flag << FLAG_SHIFT);
    return move;
}

Move_t create_move(Move_t move, int flag)
{
    move &= ~FLAG_MASK;
    move |= (flag << FLAG_SHIFT);
    return move;
}

std::vector<Move_t> create_moves_from_bitboard(Square_t start_square, Bitboard_t bitboard)
{
    std::vector<Move_t> moves;
    while (bitboard != 0)
    {
        Square_t target_square = Bitboard::pop_square(bitboard);
        moves.push_back(create_move(start_square, target_square, NO_FLAG));
    }
    return moves;
}

Square_t get_start_square(Move_t move)
{
    return (move & START_SQUARE_MASK) >> START_SQUARE_SHIFT;
}

Square_t get_target_square(Move_t move)
{
    return (move & TARGET_SQUARE_MASK) >> TARGET_SQUARE_SHIFT;
}

int get_flag(Move_t move)
{
    return (move & FLAG_MASK) >> FLAG_SHIFT;
}

bool is_pawn_promotion(Move_t move)
{
    return (move & PAWN_PROMOTION_MASK) != 0;
}

PieceType_t get_pawn_promotion_piece_type(Move_t move)
{
    switch (get_flag(move))
    {
    case PROMOTE_TO_KNIGHT_FLAG:
        return Piece::KNIGHT;
    case PROMOTE_TO_BISHOP_FLAG:
        return Piece::BISHOP;
    case PROMOTE_TO_ROOK_FLAG:
        return Piece::ROOK;
    case PROMOTE_TO_QUEEN_FLAG:
        return Piece::QUEEN;
    default:
        return Piece::NONE;
    }
}

std::string get_move_notation(Move_t move)
{
    std::string notation;
    notation += Board::get_square_notation(get_start_square(move));
    notation += Board::get_square_notation(get_target_square(move));
    return notation;
}

Move_t get_move_from_notation(std::string notation, MoveGenerator& move_generator)
{
    for (Move_t move : move_generator.get_moves())
    {
        if (get_move_notation(move) == notation)
        {
            return move;
        }
    }
    return NONE_MOVE;
}
} // namespace Move
