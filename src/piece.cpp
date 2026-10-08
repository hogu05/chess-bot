#include "piece.hpp"

#include "directions.hpp"
#include "square.hpp"
#include "types.hpp"

namespace piece
{
PieceType get_piece_type(Piece piece)
{
    return (piece & PIECE_TYPE_MASK) >> PIECE_TYPE_SHIFT;
}

Color get_piece_color(Piece piece)
{
    return (piece & PIECE_COLOR_MASK) >> PIECE_COLOR_SHIFT;
}


Piece create_piece(PieceType type, Color color)
{
    return (color << PIECE_COLOR_SHIFT) | (type << PIECE_TYPE_SHIFT);
}

bool can_pawn_move_two_spaces(Square square, Color color)
{
    int rank = square::get_rank(square);
    return (color == color::WHITE && rank == 1) || (color == color::BLACK && rank == square::RANKS - 2);
}

bool can_pawn_promote(Square square, Color color)
{
    int rank = square::get_rank(square);
    return (color == color::WHITE && rank == square::RANKS - 2) || (color == color::BLACK && rank == 1);
}

bool can_move_in_direction(PieceType piece_type, Direction direction)
{
    if ((piece_type == BISHOP || piece_type == QUEEN) &&
        directions::is_diagonal_direction(direction))
    {
        return true;
    }
    if ((piece_type == ROOK || piece_type == QUEEN) &&
        directions::is_orthogonal_direction(direction))
    {
        return true;
    }
    return false;
}

bool is_sliding_piece(PieceType piece_type)
{
    return piece_type == BISHOP || piece_type == ROOK || piece_type == QUEEN;
}

int get_piece_index(Piece piece)
{
    PieceType piece_type = get_piece_type(piece);

    if (piece_type == NONE)
    {
        return -1;
    }

    int index = piece_type - 1;
    if (get_piece_color(piece) == color::BLACK)
    {
        index += PIECE_TYPE_COUNT;
    }
    return index;
}
} // namespace piece
