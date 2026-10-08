#ifndef HASHER_H
#define HASHER_H

#include <cstdint>

#include "types.hpp"

namespace hasher
{
void update_square(std::uint64_t& hash, Square square, Piece piece);

void update_to_move(std::uint64_t& hash);

void update_castling_rights(std::uint64_t& hash, PositionInfo position_info);

void update_en_passant(std::uint64_t& hash, int file);
} // namespace hasher

#endif
