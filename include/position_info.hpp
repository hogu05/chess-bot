#ifndef POSITION_INFO_H
#define POSITION_INFO_H

#include "bit_utils.hpp"
#include "color.hpp"
#include "types.hpp"

namespace position_info
{
constexpr int TO_MOVE_SHIFT = 0;
constexpr int CAPTURED_PIECE_SHIFT = 1;
constexpr int EN_PASSANT_FLAG_SHIFT = 5;
constexpr int EN_PASSANT_FILE_SHIFT = 6;
constexpr int SHORT_CASTLE_WHITE_SHIFT = 9;
constexpr int SHORT_CASTLE_BLACK_SHIFT = 10;
constexpr int LONG_CASTLE_WHITE_SHIFT = 11;
constexpr int LONG_CASTLE_BLACK_SHIFT = 12;
constexpr int FIFTY_MOVES_PLY_SHIFT = 13;

constexpr int TO_MOVE_MASK = bit_utils::mask(TO_MOVE_SHIFT, 1);
constexpr int CAPTURED_PIECE_MASK = bit_utils::mask(CAPTURED_PIECE_SHIFT, 4);
constexpr int EN_PASSANT_FLAG_MASK = bit_utils::mask(EN_PASSANT_FLAG_SHIFT, 1);
constexpr int EN_PASSANT_FILE_MASK = bit_utils::mask(EN_PASSANT_FILE_SHIFT, 3);
constexpr int SHORT_CASTLE_WHITE_MASK = bit_utils::mask(SHORT_CASTLE_WHITE_SHIFT, 1);
constexpr int SHORT_CASTLE_BLACK_MASK = bit_utils::mask(SHORT_CASTLE_BLACK_SHIFT, 1);
constexpr int LONG_CASTLE_WHITE_MASK = bit_utils::mask(LONG_CASTLE_WHITE_SHIFT, 1);
constexpr int LONG_CASTLE_BLACK_MASK = bit_utils::mask(LONG_CASTLE_BLACK_SHIFT, 1);
constexpr int FIFTY_MOVES_PLY_MASK = bit_utils::mask(FIFTY_MOVES_PLY_SHIFT, 7);

constexpr int CASTLING_RIGHTS_SHIFT = SHORT_CASTLE_WHITE_SHIFT;
constexpr int CASTLING_RIGHTS_MASK = SHORT_CASTLE_WHITE_MASK | SHORT_CASTLE_BLACK_MASK |
                                     LONG_CASTLE_WHITE_MASK | LONG_CASTLE_BLACK_MASK;

constexpr int get_castling_mask(Color color, bool short_castle)
{
    if (short_castle)
    {
        return color == color::WHITE ? SHORT_CASTLE_WHITE_MASK : SHORT_CASTLE_BLACK_MASK;
    }
    return color == color::WHITE ? LONG_CASTLE_WHITE_MASK : LONG_CASTLE_BLACK_MASK;
}

void set_to_move(PositionInfo& position_info, Color to_move);

void set_captured_piece(PositionInfo& position_info, Piece captured_piece);

void set_en_passant(PositionInfo& position_info, bool en_passant_flag, int en_passant_file);

void set_castling_right(PositionInfo& position_info, Color color, bool short_castle,
                        bool castling_right);

void set_castling_rights(PositionInfo& position_info, int castling_rights);

void set_fifty_move_ply(PositionInfo& position_info, int fifty_move_ply);

constexpr Color get_to_move(PositionInfo position_info)
{
    return position_info & TO_MOVE_MASK;
}

Piece get_captured_piece(PositionInfo position_info);

constexpr int get_en_passant_file(PositionInfo position_info)
{
    if ((position_info & EN_PASSANT_FLAG_MASK) == 0)
    {
        return -1;
    }
    return (position_info & EN_PASSANT_FILE_MASK) >> EN_PASSANT_FILE_SHIFT;
}

constexpr bool get_castling_right(PositionInfo position_info, Color color, bool short_castle)
{
    return (position_info & get_castling_mask(color, short_castle)) != 0;
}

int get_castling_rights(PositionInfo position_info);

constexpr int get_fifty_move_ply(PositionInfo position_info)
{
    return (position_info & FIFTY_MOVES_PLY_MASK) >> FIFTY_MOVES_PLY_SHIFT;
}
}; // namespace position_info

#endif
