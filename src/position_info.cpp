#include "position_info.hpp"

#include "piece.hpp"
#include "types.hpp"

namespace position_info
{
void set_to_move(PositionInfo& position_info, Color to_move)
{
    position_info = (position_info & ~TO_MOVE_MASK) | (to_move << TO_MOVE_SHIFT);
}

void set_captured_piece(PositionInfo& position_info, Piece captured_piece)
{
    position_info =
        (position_info & ~CAPTURED_PIECE_MASK) | (captured_piece << CAPTURED_PIECE_SHIFT);
}

void set_en_passant(PositionInfo& position_info, bool en_passant_flag, int en_passant_file)
{
    position_info = (position_info & ~EN_PASSANT_FLAG_MASK) |
                    (en_passant_flag ? EN_PASSANT_FLAG_MASK : 0) |
                    (en_passant_file << EN_PASSANT_FILE_SHIFT);
}

void set_castling_right(PositionInfo& position_info, Color color, bool short_castle,
                        bool castling_right)
{
    int mask = get_castling_mask(color, short_castle);
    position_info = (position_info & ~mask) | (castling_right ? mask : 0);
}

void set_castling_rights(PositionInfo& position_info, int castling_rights)
{
    position_info = position_info & ~CASTLING_RIGHTS_MASK;
    position_info = position_info | (castling_rights << CASTLING_RIGHTS_SHIFT);
}

void set_fifty_move_ply(PositionInfo& position_info, int fifty_move_ply)
{
    position_info =
        (position_info & ~FIFTY_MOVES_PLY_MASK) | (fifty_move_ply << FIFTY_MOVES_PLY_SHIFT);
}

Piece get_captured_piece(PositionInfo position_info)
{
    return (position_info & CAPTURED_PIECE_MASK) >> CAPTURED_PIECE_SHIFT;
}

int get_castling_rights(PositionInfo position_info)
{
    return (position_info & CASTLING_RIGHTS_MASK) >> CASTLING_RIGHTS_SHIFT;
}

int get_fifty_move_ply(PositionInfo position_info)
{
    return (position_info & FIFTY_MOVES_PLY_MASK) >> FIFTY_MOVES_PLY_SHIFT;
}
} // namespace position_info
