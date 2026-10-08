#ifndef PIECE_H
#define PIECE_H

#include "bit_utils.hpp"
#include "color.hpp"
#include "square.hpp"
#include "types.hpp"

namespace piece
{
constexpr int PIECE_COLOR_SHIFT = 0;
constexpr int PIECE_TYPE_SHIFT = 1;

constexpr int PIECE_COLOR_MASK = bit_utils::mask(PIECE_COLOR_SHIFT, 1);
constexpr int PIECE_TYPE_MASK = bit_utils::mask(PIECE_TYPE_SHIFT, 3);

constexpr PieceType NONE = 0b000;
constexpr PieceType PAWN = 0b001;
constexpr PieceType KNIGHT = 0b010;
constexpr PieceType BISHOP = 0b011;
constexpr PieceType ROOK = 0b100;
constexpr PieceType QUEEN = 0b101;
constexpr PieceType KING = 0b110;
constexpr int PIECE_TYPE_COUNT = 6;

constexpr PieceType get_piece_type(Piece piece)
{
    return (piece & PIECE_TYPE_MASK) >> PIECE_TYPE_SHIFT;
}

constexpr Color get_piece_color(Piece piece)
{
    return (piece & PIECE_COLOR_MASK) >> PIECE_COLOR_SHIFT;
}

constexpr Piece create_piece(PieceType piece_type, Color color)
{
    return (color << PIECE_COLOR_SHIFT) | (piece_type << PIECE_TYPE_SHIFT);
}

constexpr bool can_pawn_move_two_spaces(Square square, Color color)
{
    const int rank = square::get_rank(square);
    return (color == color::WHITE && rank == 1) ||
           (color == color::BLACK && rank == square::RANKS - 2);
}

constexpr bool can_pawn_promote(Square square, Color color)
{
    const int rank = square::get_rank(square);
    return (color == color::WHITE && rank == square::RANKS - 2) ||
           (color == color::BLACK && rank == 1);
}

constexpr int get_piece_index(Piece piece)
{
    const PieceType piece_type = get_piece_type(piece);
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

#endif
