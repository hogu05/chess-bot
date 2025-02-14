#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <vector>
#include <string>

namespace Move {
    const int START_SQUARE_MASK =    0b000000000111111;
    const int TARGET_SQUARE_MASK =   0b000111111000000;
    const int FLAG_MASK =            0b111000000000000;
    const int PAWN_PROMOTION_MASK =  0b100000000000000;

    const int NO_FLAG = 0b000;
    const int EN_PASSANT_FLAG = 0b001;
    const int CASTLE_FLAG = 0b010;
    const int TWO_SPACE_PAWN_MOVE_FLAG = 0b011;
    const int PROMOTE_TO_KNIGHT_FLAG = 0b100;
    const int PROMOTE_TO_BISHOP_FLAG = 0b101;
    const int PROMOTE_TO_ROOK_FLAG = 0b110;
    const int PROMOTE_TO_QUEEN_FLAG = 0b111;

    int create_move(int start_square, int target_square, int flag);
    int create_move(int move, int flag);
    std::vector<int> create_moves_from_bitboard(int start_square, uint64_t bitboard);
    int get_start_square(int move);
    int get_target_square(int move);
    int get_flag(int move);

    bool is_pawn_promotion(int move);
    int get_pawn_promotion_piece_type(int move);

    std::string get_move_notation(int move);
};



#endif
