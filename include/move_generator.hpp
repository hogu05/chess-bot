#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

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

    int perft(int depth);

  private:
    Board& board;

    Bitboard friendly_pieces_bb = 0;
    Bitboard enemy_pieces_bb = 0;
    Bitboard all_pieces_bb = 0;
    Bitboard friendly_king_bb = 0;
    Square en_passant_square = -1;
    Bitboard attacked_squares_bb = 0;
    Bitboard checking_piece_bb = 0;
    Bitboard blocking_squares_bb = 0;
    Bitboard pinned_pieces_bb = 0;
    Bitboard pinned_piece_possible_squares_bb = 0;

    bool is_double_check = false;

    void add_piece_moves(MoveList& moves, Square square) const;

    void add_pawn_moves(MoveList& moves, Square square, Color color) const;

    static void add_pawn_move(MoveList& moves, Square start_square, Square target_square, int flag,
                              bool is_promotion);

    void add_knight_moves(MoveList& moves, Square square) const;

    void add_sliding_piece_moves(MoveList& moves, Square square, Piece piece) const;

    void add_king_moves(MoveList& moves, Square square, Color color) const;

    void init_bitboards();

    void update_attacks();

    Bitboard get_piece_attacks(Square square) const;

    static Bitboard get_pawn_attacks(Square square, Color color);

    static Bitboard get_knight_attacks(Square square);

    Bitboard get_sliding_piece_attacks(Square square, Piece piece) const;

    static Bitboard get_king_attacks(Square square);

    Bitboard get_pinned_pieces() const;

    Bitboard get_blocking_squares() const;

    Bitboard get_pinned_piece_possible_squares(Square square) const;

    Bitboard get_legal_squares(Bitboard moves) const;

    bool is_en_passant_legal(Square start_square, Square target_square) const;
};

#endif
