#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <array>

class Board {
public:
    static const int FILES = 8;
    static const int RANKS = 8;
    static const int TOTAL_SQUARES = FILES * RANKS;

    std::array<int, 64> pieces;
    int to_move;

    int en_passant = -1;
    std::array<bool, 2> short_castle;
    std::array<bool, 2> long_castle;

    void load_position_from_fen(std::string fen);
    void print_board();

    bool is_occupied(int square);
    bool is_empty(int square);
    int get_piece_color(int square);


    void make_move(int move);
    void unmake_move(int move);

    static int get_file(int square);
    static int get_rank(int square);

    static bool is_valid_square(int square);
    static std::string square_to_notation(int square);
};

#endif
