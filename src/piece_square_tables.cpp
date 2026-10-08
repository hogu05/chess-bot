#include "piece_square_tables.hpp"

#include <array>

#include "color.hpp"
#include "piece.hpp"
#include "types.hpp"
namespace piece_square_tables
{
int get_piece_score(Piece piece, Square square, bool is_endgame)
{
    const std::array<int, square::TOTAL_SQUARES>* table = nullptr;
    Piece piece_type = piece::get_piece_type(piece);
    Color piece_color = piece::get_piece_color(piece);

    if (piece_type == piece::NONE)
    {
        return 0;
    }

    switch (piece_type)
    {
    case piece::PAWN:
        table = &pawn;
        break;
    case piece::KNIGHT:
        table = &knight;
        break;
    case piece::BISHOP:
        table = &bishop;
        break;
    case piece::ROOK:
        table = &rook;
        break;
    case piece::QUEEN:
        table = &queen;
        break;
    case piece::KING:
        if (is_endgame)
        {
            table = &king_end;
        }
        else
        {
            table = &king_mid;
        }
        break;
    }

    Square table_square = square;

    if (piece_color == color::WHITE)
    {
        int rank = (square::RANKS - 1) - square::get_rank(square);
        table_square = square::create_square(square::get_file(square), rank);
    }

    return (*table)[table_square];
}
} // namespace piece_square_tables
