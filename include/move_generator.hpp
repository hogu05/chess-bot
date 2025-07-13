#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <vector>

#include "types.hpp"

class Board;

class MoveGenerator
{
  public:
    explicit MoveGenerator(Board &board);

    std::vector<Move_t> get_moves();

    int perft(int depth);

  private:
    Board &board;

    Bitboard_t friendly_pieces_bb = 0;
    Bitboard_t enemy_pieces_bb = 0;
    Bitboard_t all_pieces_bb = 0;
    Bitboard_t friendly_king_bb = 0;
    Square_t en_passant_square = 0;

    Bitboard_t attacked_squares_bb = 0;
    Bitboard_t checking_piece_bb = 0;
    Bitboard_t blocking_squares_bb = 0;
    Bitboard_t pinned_pieces_bb = 0;
    Bitboard_t pinned_piece_possible_squares_bb = 0;

    bool is_double_check = false;

    std::vector<Move_t> get_piece_moves(Square_t square);

    std::vector<Move_t> get_pawn_moves(Square_t square, Color_t color);

    std::vector<Move_t> get_knight_moves(Square_t square);

    std::vector<Move_t> get_sliding_piece_moves(Square_t square, Piece_t piece);

    std::vector<Move_t> get_king_moves(Square_t square, Color_t color);

    std::vector<Move_t> get_pawn_promotion_moves(std::vector<Move_t> moves);

    void init_bitboards();

    Bitboard_t get_attacked_squares_and_update_checks();

    Bitboard_t get_piece_attacks(Square_t square);

    Bitboard_t get_pawn_attacks(Square_t square, Color_t color);

    Bitboard_t get_knight_attacks(Square_t square);

    Bitboard_t get_sliding_piece_attacks(Square_t square, Piece_t piece);

    Bitboard_t get_king_attacks(Square_t square);

    Bitboard_t get_pinned_pieces();

    Bitboard_t get_blocking_squares();

    Bitboard_t get_pinned_piece_possible_squares(Square_t square);

    Bitboard_t get_legal_squares(Bitboard_t moves);

    bool is_en_passant_legal(Square_t start_square, Square_t target_square);
};

#endif
