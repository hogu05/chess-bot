#include "position_info.hpp"

#include "piece.hpp"
#include <iostream>

void PositionInfo::set_to_move(int &position_info, int to_move) {
    position_info = (position_info & ~TO_MOVE_MASK) | to_move;
}

void PositionInfo::set_captured_piece(int& position_info, int captured_piece_type) {
    position_info = (position_info & ~CAPTURED_PIECE_MASK) | (captured_piece_type << 1);
}

void PositionInfo::set_en_passant(int& position_info, bool en_passant_flag, int en_passant_file) {
    position_info = (position_info & ~EN_PASSANT_FLAG_MASK) | (en_passant_flag << 5) | (en_passant_file << 6);
}

void PositionInfo::set_castling_right(int& position_info, int color, bool short_castle, bool castling_right) {
    if (short_castle) {
        if (color == Piece::WHITE) {
            position_info = (position_info & ~SHORT_CASTLE_WHITE_MASK) | (castling_right << 9);
        } else {
            position_info = (position_info & ~SHORT_CASTLE_BLACK_MASK) | (castling_right << 10);
        }
    } else {
        if (color == Piece::WHITE) {
            position_info = (position_info & ~LONG_CASTLE_WHITE_MASK) | (castling_right << 11);
        } else {
            position_info = (position_info & ~LONG_CASTLE_BLACK_MASK) | (castling_right << 12);
        }
    }
}

void PositionInfo::set_castling_rights(int &position_info, int castling_rights) {
    position_info = position_info & ~(SHORT_CASTLE_WHITE_MASK | SHORT_CASTLE_BLACK_MASK | LONG_CASTLE_WHITE_MASK | LONG_CASTLE_BLACK_MASK);
    position_info = position_info | (castling_rights << 9);
}

void PositionInfo::set_fifty_move_ply(int &position_info, int fifty_move_ply) {
    position_info = (position_info & ~FIFTY_MOVES_PLY_MASK) | (fifty_move_ply << 13);
}



int PositionInfo::get_to_move(int position_info) {
    return position_info & TO_MOVE_MASK;
}

int PositionInfo::get_captured_piece(int position_info) {
    return (position_info & CAPTURED_PIECE_MASK) >> 1;
}

int PositionInfo::get_en_passant_file(int position_info) {
    if ((position_info & EN_PASSANT_FLAG_MASK) == 0) {
        return -1;
    }
    return (position_info & EN_PASSANT_FILE_MASK) >> 6;
}

bool PositionInfo::get_castling_right(int position_info, int color, bool short_castle) {
    if (short_castle) {
        if (color == Piece::WHITE) {
            return (position_info & SHORT_CASTLE_WHITE_MASK) >> 9;
        } else {
            return (position_info & SHORT_CASTLE_BLACK_MASK) >> 10;
        }
    } else {
        if (color == Piece::WHITE) {
            return (position_info & LONG_CASTLE_WHITE_MASK) >> 11;
        } else {
            return (position_info & LONG_CASTLE_BLACK_MASK) >> 12;
        }
    }
}

int PositionInfo::get_castling_rights(int position_info) {
    return (position_info &
        (SHORT_CASTLE_WHITE_MASK | SHORT_CASTLE_BLACK_MASK | LONG_CASTLE_WHITE_MASK | LONG_CASTLE_BLACK_MASK)) >> 9;
}

int PositionInfo::get_fifty_move_ply(int position_info) {
    return (position_info & FIFTY_MOVES_PLY_MASK) >> 13;
}
