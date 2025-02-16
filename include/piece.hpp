#ifndef PIECE_H
#define PIECE_H

namespace Piece {
    constexpr int NONE = 0b0000;
    constexpr int PAWN = 0b0010;
    constexpr int KNIGHT = 0b0100;
    constexpr int BISHOP = 0b0110;
    constexpr int ROOK = 0b1000;
    constexpr int QUEEN = 0b1010;
    constexpr int KING = 0b1100;
    constexpr int PIECE_TYPE_MASK = 0b1110;

    constexpr int WHITE = 0b0000;
    constexpr int BLACK = 0b0001;
    constexpr int PIECE_COLOR_MASK = 0b0001;

    constexpr int WHITE_PAWN = PAWN | WHITE;
    constexpr int WHITE_KNIGHT = KNIGHT | WHITE;
    constexpr int WHITE_BISHOP = BISHOP | WHITE;
    constexpr int WHITE_ROOK = ROOK | WHITE;
    constexpr int WHITE_QUEEN = QUEEN | WHITE;
    constexpr int WHITE_KING = KING | WHITE;
    constexpr int BLACK_PAWN = PAWN | BLACK;
    constexpr int BLACK_KNIGHT = KNIGHT | BLACK;
    constexpr int BLACK_BISHOP = BISHOP | BLACK;
    constexpr int BLACK_ROOK = ROOK | BLACK;
    constexpr int BLACK_QUEEN = QUEEN | BLACK;
    constexpr int BLACK_KING = KING | BLACK;

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
