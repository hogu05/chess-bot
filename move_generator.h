#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include <vector>
#include "board.h"
#include "directions.h"

class MoveGenerator {
public:
    MoveGenerator(Board board);
    std::vector<int> generate_moves();
private:
    Board board;

    std::vector<int> generate_piece_moves(int piece, int square, int color);
    std::vector<int> generate_pawn_moves(int square, int color);
    std::vector<int> generate_knight_moves(int square, int color);
    std::vector<int> generate_sliding_piece_moves(int square, int piece, int color);
    std::vector<int> generate_king_moves(int square, int color);

    std::vector<int> generate_pawn_promotion_moves(std::vector<int> moves);
};



#endif
