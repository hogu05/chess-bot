#include "position_info.hpp"

#include "piece.hpp"
#include "types.hpp"

void PositionInfo::set_to_move(PositionInfo_t& position_info, Color_t to_move)
{
    position_info = (position_info & ~TO_MOVE_MASK) | (to_move << TO_MOVE_SHIFT);
}

void PositionInfo::set_captured_piece(PositionInfo_t& position_info, Piece_t captured_piece)
{
    position_info =
        (position_info & ~CAPTURED_PIECE_MASK) | (captured_piece << CAPTURED_PIECE_SHIFT);
}

void PositionInfo::set_en_passant(PositionInfo_t& position_info, bool en_passant_flag,
                                  int en_passant_file)
{
    position_info = (position_info & ~EN_PASSANT_FLAG_MASK) |
                    (en_passant_flag << EN_PASSANT_FLAG_SHIFT) |
                    (en_passant_file << EN_PASSANT_FILE_SHIFT);
}

void PositionInfo::set_castling_right(PositionInfo_t& position_info, Color_t color,
                                      bool short_castle, bool castling_right)
{
    if (short_castle)
    {
        if (color == Piece::WHITE)
        {
            position_info = (position_info & ~SHORT_CASTLE_WHITE_MASK) |
                            (castling_right << SHORT_CASTLE_WHITE_SHIFT);
        }
        else
        {
            position_info = (position_info & ~SHORT_CASTLE_BLACK_MASK) |
                            (castling_right << SHORT_CASTLE_BLACK_SHIFT);
        }
    }
    else
    {
        if (color == Piece::WHITE)
        {
            position_info = (position_info & ~LONG_CASTLE_WHITE_MASK) |
                            (castling_right << LONG_CASTLE_WHITE_SHIFT);
        }
        else
        {
            position_info = (position_info & ~LONG_CASTLE_BLACK_MASK) |
                            (castling_right << LONG_CASTLE_BLACK_SHIFT);
        }
    }
}

void PositionInfo::set_castling_rights(PositionInfo_t& position_info, int castling_rights)
{
    position_info = position_info & ~CASTLING_RIGHTS_MASK;
    position_info = position_info | (castling_rights << CASTLING_RIGHTS_SHIFT);
}

void PositionInfo::set_fifty_move_ply(PositionInfo_t& position_info, int fifty_move_ply)
{
    position_info =
        (position_info & ~FIFTY_MOVES_PLY_MASK) | (fifty_move_ply << FIFTY_MOVES_PLY_SHIFT);
}

Color_t PositionInfo::get_to_move(PositionInfo_t position_info)
{
    return position_info & TO_MOVE_MASK;
}

Piece_t PositionInfo::get_captured_piece(PositionInfo_t position_info)
{
    return (position_info & CAPTURED_PIECE_MASK) >> CAPTURED_PIECE_SHIFT;
}

int PositionInfo::get_en_passant_file(PositionInfo_t position_info)
{
    if ((position_info & EN_PASSANT_FLAG_MASK) == 0)
    {
        return -1;
    }
    return (position_info & EN_PASSANT_FILE_MASK) >> EN_PASSANT_FILE_SHIFT;
}

bool PositionInfo::get_castling_right(PositionInfo_t position_info, Color_t color,
                                      bool short_castle)
{
    if (short_castle)
    {
        if (color == Piece::WHITE)
        {
            return (position_info & SHORT_CASTLE_WHITE_MASK) >> SHORT_CASTLE_WHITE_SHIFT;
        }
        else
        {
            return (position_info & SHORT_CASTLE_BLACK_MASK) >> SHORT_CASTLE_BLACK_SHIFT;
        }
    }
    else
    {
        if (color == Piece::WHITE)
        {
            return (position_info & LONG_CASTLE_WHITE_MASK) >> LONG_CASTLE_WHITE_SHIFT;
        }
        else
        {
            return (position_info & LONG_CASTLE_BLACK_MASK) >> LONG_CASTLE_BLACK_SHIFT;
        }
    }
}

int PositionInfo::get_castling_rights(PositionInfo_t position_info)
{
    return (position_info & CASTLING_RIGHTS_MASK) >> CASTLING_RIGHTS_SHIFT;
}

int PositionInfo::get_fifty_move_ply(PositionInfo_t position_info)
{
    return (position_info & FIFTY_MOVES_PLY_MASK) >> FIFTY_MOVES_PLY_SHIFT;
}
