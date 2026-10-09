#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <cstdint>

#include "move_list.hpp"
#include "types.hpp"

class Board;

class MoveGenerator
{
  public:
    explicit MoveGenerator(Board& board);
    MoveGenerator(const MoveGenerator&) = delete;
    MoveGenerator& operator=(const MoveGenerator&) = delete;

    MoveList get_moves();

    bool is_check() const;

    std::uint64_t perft(int depth);

  private:
    Board& board;

    Bitboard attacked_squares = 0;
    Bitboard checkers = 0;
    Bitboard blocking_squares = 0;
    Bitboard pinned_pieces = 0;
    Bitboard pinned_piece_possible_squares = 0;
    bool is_double_check = false;

    Bitboard get_friendly_pieces() const;

    Bitboard get_enemy_pieces() const;

    Square get_friendly_king_square() const;

    void update_checks_and_pins();

    void update_attacks();

    Bitboard get_pinned_pieces() const;

    void update_pinned_piece_possible_squares(Square square);

    Bitboard get_legal_squares(Bitboard squares) const;

    void add_king_moves(MoveList& moves, Square square, Color color) const;

    void add_pawn_moves(MoveList& moves, Square square, Color color) const;

    static void add_pawn_move(MoveList& moves, Square start_square, Square target_square, int flag,
                              bool is_promotion);

    void add_knight_moves(MoveList& moves, Square square) const;

    void add_sliding_piece_moves(MoveList& moves, Square square, PieceType piece_type) const;

    bool is_en_passant_legal(Square start_square, Square target_square) const;
};

#endif
