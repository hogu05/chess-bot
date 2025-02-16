#ifndef POSITION_INFO_H
#define POSITION_INFO_H

namespace PositionInfo {
    constexpr int TO_MOVE_MASK = 0b00000000000000000001;            // 00000000000000000001
    constexpr int CAPTURED_PIECE_MASK = 0b00000000000000011110;     // 00000000000000011110
    constexpr int EN_PASSANT_FLAG_MASK = 0b00000000000000100000;    // 00000000000000100000
    constexpr int EN_PASSANT_FILE_MASK = 0b00000000000111000000;    // 00000000000111000000
    constexpr int SHORT_CASTLE_WHITE_MASK = 0b00000000001000000000; // 00000000001000000000
    constexpr int SHORT_CASTLE_BLACK_MASK = 0b00000000010000000000; // 00000000010000000000
    constexpr int LONG_CASTLE_WHITE_MASK = 0b00000000100000000000;  // 00000000100000000000
    constexpr int LONG_CASTLE_BLACK_MASK = 0b00000001000000000000;  // 00000001000000000000
    constexpr int FIFTY_MOVES_PLY_MASK = 0b11111110000000000000;    // 11111110000000000000

    void set_to_move(int& position_info, int to_move);
    void set_captured_piece(int& position_info, int captured_piece);
    void set_en_passant(int& position_info, bool en_passant_flag, int en_passant_file);
    void set_castling_right(int& position_info, int color, bool short_castle, bool castling_right);
    void set_castling_rights(int& position_info, int castling_rights);
    void set_fifty_move_ply(int& position_info, int fifty_move_ply);

    int get_to_move(int position_info);
    int get_captured_piece(int position_info);
    int get_en_passant_file(int position_info);
    bool get_castling_right(int position_info, int color, bool short_castle);
    int get_castling_rights(int position_info);
    int get_fifty_move_ply(int position_info);
};



#endif
