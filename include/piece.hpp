#ifndef PIECE_H
#define PIECE_H

namespace Piece {
    const int NONE = 0b0000;
    const int PAWN = 0b0010;
    const int KNIGHT = 0b0100;
    const int BISHOP = 0b0110;
    const int ROOK = 0b1000;
    const int QUEEN = 0b1010;
    const int KING = 0b1100;
    const int PIECE_TYPE_MASK = 0b1110;

    const int WHITE = 0b0000;
    const int BLACK = 0b0001;
    const int PIECE_COLOR_MASK = 0b0001;

    const int WHITE_PAWN = PAWN | WHITE;
    const int WHITE_KNIGHT = KNIGHT | WHITE;
    const int WHITE_BISHOP = BISHOP | WHITE;
    const int WHITE_ROOK = ROOK | WHITE;
    const int WHITE_QUEEN = QUEEN | WHITE;
    const int WHITE_KING = KING | WHITE;
    const int BLACK_PAWN = PAWN | BLACK;
    const int BLACK_KNIGHT = KNIGHT | BLACK;
    const int BLACK_BISHOP = BISHOP | BLACK;
    const int BLACK_ROOK = ROOK | BLACK;
    const int BLACK_QUEEN = QUEEN | BLACK;
    const int BLACK_KING = KING | BLACK;

    int get_piece_type(int piece_index);
    int get_piece_color(int piece_index);

    char get_piece_symbol(int piece_index);
    int get_piece_from_symbol(char symbol);

    int get_other_color(int color);

    int create_piece(int piece_type, int color);

    bool can_pawn_move_two_spaces(int square, int color);
    bool can_pawn_promote(int square, int color);

    bool can_move_in_direction(int piece, int direction);
    bool is_sliding_piece(int piece);
};

#endif
