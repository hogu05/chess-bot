#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <cstdint>
#include <vector>
#include <array>
#include "board.h"
#include "piece.h"

class MoveGenerator {
public:
    MoveGenerator(Board board);
    std::vector<int> generate_moves();
private:
    Board board;

    uint64_t friendly_pieces_bitboard;
    uint64_t enemy_pieces_bitboard;
    uint64_t all_pieces_bitboard;
    uint64_t friendly_king_bitboard;

    uint64_t attacked_squares_bitboard;
    uint64_t checking_piece_bitboard;
    uint64_t blocking_squares_bitboard;
    uint64_t pinned_pieces_bitboard;
    uint64_t pinned_piece_possible_squares_bitboard;
    bool is_double_check;

    void generate_bitboards();
    std::vector<int> generate_piece_moves(int square);
    std::vector<int> generate_pawn_moves(int square, int color);
    std::vector<int> generate_knight_moves(int square);
    std::vector<int> generate_sliding_piece_moves(int square, int piece);
    std::vector<int> generate_king_moves(int square, int color);

    std::vector<int> generate_pawn_promotion_moves(std::vector<int> moves);

    void generate_attacked_squares_bitboard();
    uint64_t generate_piece_attacks(int square);
    uint64_t generate_pawn_attacks(int square, int color);
    uint64_t generate_knight_attacks(int square);
    uint64_t generate_sliding_piece_attacks(int square, int piece);
    uint64_t generate_king_attacks(int square);

    void generate_pinned_pieces_bitboard();
    void generate_blocking_squares_bitboard();
    void generate_pinned_piece_possible_squares_bitboard(int square);

    uint64_t get_legal_moves(uint64_t moves);
};



#endif
