#ifndef POSITION_INFO_H
#define POSITION_INFO_H

#include "types.hpp"

namespace PositionInfo
{
    constexpr int TO_MOVE_MASK = 0b00000000000000000001; // 00000000000000000001
    constexpr int CAPTURED_PIECE_MASK = 0b00000000000000011110; // 00000000000000011110
    constexpr int EN_PASSANT_FLAG_MASK = 0b00000000000000100000; // 00000000000000100000
    constexpr int EN_PASSANT_FILE_MASK = 0b00000000000111000000; // 00000000000111000000
    constexpr int SHORT_CASTLE_WHITE_MASK = 0b00000000001000000000; // 00000000001000000000
    constexpr int SHORT_CASTLE_BLACK_MASK = 0b00000000010000000000; // 00000000010000000000
    constexpr int LONG_CASTLE_WHITE_MASK = 0b00000000100000000000; // 00000000100000000000
    constexpr int LONG_CASTLE_BLACK_MASK = 0b00000001000000000000; // 00000001000000000000
    constexpr int FIFTY_MOVES_PLY_MASK = 0b11111110000000000000; // 11111110000000000000

    void set_to_move(PositionInfo_t &position_info, Color_t to_move);

    void set_captured_piece(PositionInfo_t &position_info, Piece_t captured_piece);

    void set_en_passant(PositionInfo_t &position_info, bool en_passant_flag, int en_passant_file);

    void set_castling_right(PositionInfo_t &position_info, Color_t color, bool short_castle, bool castling_right);

    void set_castling_rights(PositionInfo_t &position_info, int castling_rights);

    void set_fifty_move_ply(PositionInfo_t &position_info, int fifty_move_ply);

    Color_t get_to_move(PositionInfo_t position_info);

    Piece_t get_captured_piece(PositionInfo_t position_info);

    int get_en_passant_file(PositionInfo_t position_info);

    bool get_castling_right(PositionInfo_t position_info, Color_t color, bool short_castle);

    int get_castling_rights(PositionInfo_t position_info);

    int get_fifty_move_ply(PositionInfo_t position_info);
};


#endif
