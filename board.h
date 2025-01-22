#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <array>
#include "position_info.h"
#include <stack>

class Board {
public:
    static const int FILES = 8;
    static const int RANKS = 8;
    static const int TOTAL_SQUARES = FILES * RANKS;
    enum Square {
        a1, b1, c1, d1, e1, f1, g1, h1,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a8, b8, c8, d8, e8, f8, g8, h8
    };

    static int get_file(int square);
    static int get_rank(int square);
    static int get_square(int file, int rank);
    static bool is_valid_square(int square);
    static int get_file_from_notation(char notation);
    static int get_square_from_notation(std::string notation);

    static constexpr std::array<int, 2> KING_START_SQUARE = {
        e1,
        e8,
    };
    static constexpr std::array<int, 2> QUEENSIDE_ROOK_START_SQUARE = {
        a1,
        a8
    };
    static constexpr std::array<int, 2> KINGSIDE_ROOK_START_SQUARE = {
        h1,
        h8
    };

    std::array<int, 64> pieces;
    PositionInfo position_info;
    std::stack<PositionInfo> previous_positions;

    void load_position_from_fen(std::string fen);
    void print_board();

    bool is_occupied(int square);
    bool is_empty(int square);
    int get_piece_color(int square);

    void move_piece(int start_square, int target_square);
    void make_move(int move);
    void unmake_move(int move);

};

#endif
