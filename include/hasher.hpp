#ifndef HASHER_H
#define HASHER_H

#include "types.hpp"

namespace hasher
{
void update_square(uint64_t& hash, Square square, Piece piece);
void update_to_move(uint64_t& hash);
void update_castling_rights(uint64_t& hash, PositionInfo position_info);
void update_en_passant(uint64_t& hash, int file);
}; // namespace hasher

#endif
