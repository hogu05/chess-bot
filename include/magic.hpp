#ifndef MAGIC_H
#define MAGIC_H

#include "types.hpp"

namespace magic
{
Bitboard get_slider_attacks(Square square, PieceType piece_type, Bitboard pieces);
} // namespace magic

#endif
