#ifndef PIECE_H
#define PIECE_H

#include "bit_utils.hpp"
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

constexpr Color WHITE = 0;
constexpr Color BLACK = 1;
constexpr int COLORS = 2;

PieceType get_piece_type(Piece piece);

Color get_piece_color(Piece piece);

Color get_other_color(Color color);

Piece create_piece(PieceType piece_type, Color color);

bool can_pawn_move_two_spaces(Square square, Color color);

bool can_pawn_promote(Square square, Color color);

bool can_move_in_direction(Piece piece, Direction direction);

bool is_sliding_piece(Piece piece);

int get_piece_index(Piece piece);
}; // namespace piece

#endif
