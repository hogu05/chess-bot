#ifndef NOTATION_H
#define NOTATION_H

#include <string>
#include <string_view>

#include "move_generator.hpp"
#include "types.hpp"

namespace notation
{
constexpr std::string_view STARTING_POSITION_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// FEN piece letters: uppercase = white, lowercase = black ('N' = white knight, 'q' = black queen)
Piece get_piece_from_letter(char letter);
char get_piece_letter(PieceType piece_type);

std::string get_move_notation(Move move);
Move get_move_from_notation(std::string_view notation, MoveGenerator& move_generator);
} // namespace notation

#endif
