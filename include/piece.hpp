#ifndef PIECE_H
#define PIECE_H

#include "types.hpp"

namespace Piece
{
constexpr PieceType_t NONE = 0b0000;
constexpr PieceType_t PAWN = 0b0010;
constexpr PieceType_t KNIGHT = 0b0100;
constexpr PieceType_t BISHOP = 0b0110;
constexpr PieceType_t ROOK = 0b1000;
constexpr PieceType_t QUEEN = 0b1010;
constexpr PieceType_t KING = 0b1100;
constexpr int PIECE_TYPE_MASK = 0b1110;

constexpr Color_t WHITE = 0b0000;
constexpr Color_t BLACK = 0b0001;
constexpr int PIECE_COLOR_MASK = 0b0001;

constexpr Piece_t WHITE_PAWN = PAWN | WHITE;
constexpr Piece_t WHITE_KNIGHT = KNIGHT | WHITE;
constexpr Piece_t WHITE_BISHOP = BISHOP | WHITE;
constexpr Piece_t WHITE_ROOK = ROOK | WHITE;
constexpr Piece_t WHITE_QUEEN = QUEEN | WHITE;
constexpr Piece_t WHITE_KING = KING | WHITE;
constexpr Piece_t BLACK_PAWN = PAWN | BLACK;
constexpr Piece_t BLACK_KNIGHT = KNIGHT | BLACK;
constexpr Piece_t BLACK_BISHOP = BISHOP | BLACK;
constexpr Piece_t BLACK_ROOK = ROOK | BLACK;
constexpr Piece_t BLACK_QUEEN = QUEEN | BLACK;
constexpr Piece_t BLACK_KING = KING | BLACK;

PieceType_t get_piece_type(Piece_t piece);

Color_t get_piece_color(Piece_t piece);

char get_piece_symbol(Piece_t piece);

Piece_t get_piece_from_symbol(char symbol);

Color_t get_other_color(Color_t color);

Piece_t create_piece(PieceType_t piece_type, Color_t color);

bool can_pawn_move_two_spaces(Square_t square, Color_t color);

bool can_pawn_promote(Square_t square, Color_t color);

bool can_move_in_direction(Piece_t piece, Direction_t direction);

bool is_sliding_piece(Piece_t piece);

int get_piece_value(Piece_t piece);
}; // namespace Piece

#endif
