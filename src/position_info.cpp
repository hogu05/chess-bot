#include "position_info.hpp"

#include "piece.hpp"

void PositionInfo::set_to_move(PositionInfo_t &position_info, Color_t to_move)
{
    position_info = (position_info & ~TO_MOVE_MASK) | to_move;
}

void PositionInfo::set_captured_piece(PositionInfo_t &position_info, Piece_t captured_piece)
{
    position_info = (position_info & ~CAPTURED_PIECE_MASK) | (captured_piece << 1);
}

void PositionInfo::set_en_passant(PositionInfo_t &position_info, bool en_passant_flag,
                                  int en_passant_file)
{
    position_info =
        (position_info & ~EN_PASSANT_FLAG_MASK) | (en_passant_flag << 5) | (en_passant_file << 6);
}

void PositionInfo::set_castling_right(PositionInfo_t &position_info, Color_t color,
                                      bool short_castle, bool castling_right)
{
    if (short_castle)
    {
        if (color == Piece::WHITE)
        {
            position_info = (position_info & ~SHORT_CASTLE_WHITE_MASK) | (castling_right << 9);
        }
        else
        {
            position_info = (position_info & ~SHORT_CASTLE_BLACK_MASK) | (castling_right << 10);
        }
    }
    else
    {
        if (color == Piece::WHITE)
        {
            position_info = (position_info & ~LONG_CASTLE_WHITE_MASK) | (castling_right << 11);
        }
        else
        {
            position_info = (position_info & ~LONG_CASTLE_BLACK_MASK) | (castling_right << 12);
        }
    }
}

void PositionInfo::set_castling_rights(PositionInfo_t &position_info, int castling_rights)
{
    position_info = position_info & ~(SHORT_CASTLE_WHITE_MASK | SHORT_CASTLE_BLACK_MASK |
                                      LONG_CASTLE_WHITE_MASK | LONG_CASTLE_BLACK_MASK);
    position_info = position_info | (castling_rights << 9);
}

void PositionInfo::set_fifty_move_ply(PositionInfo_t &position_info, int fifty_move_ply)
{
    position_info = (position_info & ~FIFTY_MOVES_PLY_MASK) | (fifty_move_ply << 13);
}

Move_t PositionInfo::get_to_move(PositionInfo_t position_info)
{
    return position_info & TO_MOVE_MASK;
}

Piece_t PositionInfo::get_captured_piece(PositionInfo_t position_info)
{
    return (position_info & CAPTURED_PIECE_MASK) >> 1;
}

int PositionInfo::get_en_passant_file(PositionInfo_t position_info)
{
    if ((position_info & EN_PASSANT_FLAG_MASK) == 0)
    {
        return -1;
    }
    return (position_info & EN_PASSANT_FILE_MASK) >> 6;
}

bool PositionInfo::get_castling_right(PositionInfo_t position_info, Color_t color,
                                      bool short_castle)
{
    if (short_castle)
    {
        if (color == Piece::WHITE)
        {
            return (position_info & SHORT_CASTLE_WHITE_MASK) >> 9;
        }
        else
        {
            return (position_info & SHORT_CASTLE_BLACK_MASK) >> 10;
        }
    }
    else
    {
        if (color == Piece::WHITE)
        {
            return (position_info & LONG_CASTLE_WHITE_MASK) >> 11;
        }
        else
        {
            return (position_info & LONG_CASTLE_BLACK_MASK) >> 12;
        }
    }
}

int PositionInfo::get_castling_rights(PositionInfo_t position_info)
{
    return (position_info & (SHORT_CASTLE_WHITE_MASK | SHORT_CASTLE_BLACK_MASK |
                             LONG_CASTLE_WHITE_MASK | LONG_CASTLE_BLACK_MASK)) >>
           9;
}

int PositionInfo::get_fifty_move_ply(PositionInfo_t position_info)
{
    return (position_info & FIFTY_MOVES_PLY_MASK) >> 13;
}
