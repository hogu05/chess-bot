#ifndef HASHER_H
#define HASHER_H

#include "types.hpp"

namespace Hasher
{
void init_hasher();
void update_square(uint64_t& hash, Square_t square, Piece_t piece);
void update_to_move(uint64_t& hash, Color_t color);
void update_castling_rights(uint64_t& hash, PositionInfo_t position_info);
void update_en_passant(uint64_t& hash, int file);
}; // namespace Hasher

#endif
