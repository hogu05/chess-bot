#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <stack>
#include <string>

#include "types.hpp"

class Board
{
  public:
    static constexpr int FILES = 8;
    static constexpr int RANKS = 8;
    static constexpr int COLORS = 2;
    static constexpr char STARTING_POSITION_FEN[] =
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    static constexpr int TOTAL_SQUARES = FILES * RANKS;

    static int get_file(Square_t square);

    static int get_rank(Square_t square);

    static Square_t get_square(int file, int rank);

    static bool is_valid_square(Square_t square);

    static std::string get_square_notation(Square_t square);

    static Square_t get_en_passant_square(int en_passant_file, Color_t to_move);

    static Square_t get_en_passant_capture_square(Square_t start_square, Square_t target_square);

    std::array<Piece_t, FILES * RANKS> pieces;
    PositionInfo_t position_info;
    std::stack<PositionInfo_t> previous_positions;

    void load_position(std::string fen);

    bool is_occupied(Square_t square);

    bool is_empty(Square_t square);

    Color_t get_piece_color(Square_t square);

    PieceType_t get_piece_type(Square_t square);

    void make_move(Move_t move);

    void unmake_move(Move_t move);

    Piece_t get_captured_piece(Move_t move);

    std::string to_string(DisplayMode display_mode);

  private:
    // clang-format off
    enum Square : Square_t {
        a1, b1, c1, d1, e1, f1, g1, h1,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a8, b8, c8, d8, e8, f8, g8, h8
    };
    // clang-format on;

    static constexpr std::array<Square_t, COLORS> KING_START_SQUARE = {e1, e8};
    static constexpr std::array<Square_t, COLORS> QUEENSIDE_ROOK_START_SQUARE = {a1, a8};
    static constexpr std::array<Square_t, COLORS> KINGSIDE_ROOK_START_SQUARE = {h1, h8};

    static int get_file_from_notation(char notation);

    static char get_file_notation(int file);

    static Square_t get_square_from_notation(std::string notation);

    void move_piece(Square_t start_square, Square_t target_square);

    void reset();
};

#endif
