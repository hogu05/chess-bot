#include "notation.hpp"

#include <cctype>

#include "color.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"

namespace notation
{
namespace
{
std::string get_square_notation(Square square)
{
    std::string notation;
    notation += static_cast<char>('a' + square::get_file(square));
    notation += static_cast<char>('1' + square::get_rank(square));
    return notation;
}
} // namespace

Piece get_piece_from_letter(char letter)
{
    PieceType piece_type = 0;
    switch (tolower(letter))
    {
    case 'p':
        piece_type = piece::PAWN;
        break;
    case 'n':
        piece_type = piece::KNIGHT;
        break;
    case 'b':
        piece_type = piece::BISHOP;
        break;
    case 'r':
        piece_type = piece::ROOK;
        break;
    case 'q':
        piece_type = piece::QUEEN;
        break;
    case 'k':
        piece_type = piece::KING;
        break;
    default:
        piece_type = piece::NONE;
    }

    Color piece_color = (std::isupper(letter) != 0) ? color::WHITE : color::BLACK;

    return piece::create_piece(piece_type, piece_color);
}

char get_piece_letter(PieceType piece_type)
{
    switch (piece_type)
    {
    case piece::PAWN:
        return 'p';
    case piece::KNIGHT:
        return 'n';
    case piece::BISHOP:
        return 'b';
    case piece::ROOK:
        return 'r';
    case piece::QUEEN:
        return 'q';
    case piece::KING:
        return 'k';
    default:
        return '?';
    }
}

std::string get_move_notation(Move move)
{
    std::string notation = get_square_notation(move::get_start_square(move)) +
                           get_square_notation(move::get_target_square(move));
    if (move::is_pawn_promotion(move))
    {
        notation += get_piece_letter(move::get_pawn_promotion_piece_type(move));
    }
    return notation;
}

Move get_move_from_notation(std::string_view notation, MoveGenerator& move_generator)
{
    for (Move move : move_generator.get_moves())
    {
        if (get_move_notation(move) == notation)
        {
            return move;
        }
    }
    return move::NONE_MOVE;
}
} // namespace notation
