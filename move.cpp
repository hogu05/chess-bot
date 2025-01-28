#include "move.h"

#include <iostream>
#include <ostream>

#include "piece.h"
#include "bitboard.h"

int Move::create_move(int start_square, int target_square, int flag) {
    int move = 0;
    move = move | start_square;
    move = move | (target_square << 6);
    move = move | (flag << 12);
    return move;
}

int Move::create_move(int move, int flag) {
    move &= ~FLAG_MASK;
    move |= (flag << 12);
    return move;
}

std::vector<int> Move::create_moves_from_bitboard(int start_square, uint64_t bitboard) {
    std::vector<int> moves;
    while (bitboard != 0) {
        int target_square = Bitboard::pop_square(bitboard);
        moves.push_back(create_move(start_square, target_square, NO_FLAG));
    }
    return moves;
}

int Move::get_start_square(int move) {
    return move & START_SQUARE_MASK;
}

int Move::get_target_square(int move) {
    return (move & TARGET_SQUARE_MASK) >> 6;
}

int Move::get_flag(int move) {
    return (move & FLAG_MASK) >> 12;
}

bool Move::is_pawn_promotion(int move) {
    return (move & PAWN_PROMOTION_MASK) != 0;
}

int Move::get_pawn_promotion_piece_type(int move) {
    switch (get_flag(move)) {
        case PROMOTE_TO_KNIGHT_FLAG:
            return Piece::KNIGHT;
        case PROMOTE_TO_BISHOP_FLAG:
            return Piece::BISHOP;
        case PROMOTE_TO_ROOK_FLAG:
            return Piece::ROOK;
        case PROMOTE_TO_QUEEN_FLAG:
            return Piece::QUEEN;
        default:
            return Piece::QUEEN;
    }
}