#ifndef PIECE_H
#define PIECE_H

#include <array>

class Piece {
public:
    static const int NONE = 0b0000;
    static const int PAWN = 0b0010;
    static const int KNIGHT = 0b0100;
    static const int BISHOP = 0b0110;
    static const int ROOK = 0b1000;
    static const int QUEEN = 0b1010;
    static const int KING = 0b1100;
    static const int PIECE_TYPE_MASK = 0b1110;

    static const int WHITE = 0b0000;
    static const int BLACK = 0b0001;
    static const int PIECE_COLOR_MASK = 0b0001;

    static const int WHITE_PAWN = PAWN | WHITE;
    static const int WHITE_KNIGHT = KNIGHT | WHITE;
    static const int WHITE_BISHOP = BISHOP | WHITE;
    static const int WHITE_ROOK = ROOK | WHITE;
    static const int WHITE_QUEEN = QUEEN | WHITE;
    static const int WHITE_KING = KING | WHITE;
    static const int BLACK_PAWN = PAWN | BLACK;
    static const int BLACK_KNIGHT = KNIGHT | BLACK;
    static const int BLACK_BISHOP = BISHOP | BLACK;
    static const int BLACK_ROOK = ROOK | BLACK;
    static const int BLACK_QUEEN = QUEEN | BLACK;
    static const int BLACK_KING = KING | BLACK;

    static int get_piece_type(int piece_index);
    static int get_piece_color(int piece_index);

    static char get_piece_symbol(int piece_index);
    static int get_piece_from_symbol(char symbol);

    static int create_piece(int piece_type, int color);

    static bool can_pawn_move_two_spaces(int square, int color);
    static bool can_pawn_promote(int square, int color);
};



#endif
