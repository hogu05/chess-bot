#include "piece_square_tables.hpp"

#include <array>

#include "piece.hpp"
#include "types.hpp"
namespace PieceSquareTables
{
int get_piece_score(Piece_t piece, Square_t square, bool is_endgame)
{
    const std::array<int, Board::TOTAL_SQUARES>* table;
    Piece_t piece_type = Piece::get_piece_type(piece);
    Color_t piece_color = Piece::get_piece_color(piece);

    if (piece_type == Piece::NONE)
    {
        return 0;
    }

    switch (piece_type)
    {
    case Piece::PAWN:
        table = &pawn;
        break;
    case Piece::KNIGHT:
        table = &knight;
        break;
    case Piece::BISHOP:
        table = &bishop;
        break;
    case Piece::ROOK:
        table = &rook;
        break;
    case Piece::QUEEN:
        table = &queen;
        break;
    case Piece::KING:
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

    Square_t table_square = square;

    if (piece_color == Piece::WHITE)
    {
        int rank = (Board::RANKS - 1) - Board::get_rank(square);
        table_square = Board::get_square(Board::get_file(square), rank);
    }

    return (*table)[table_square];
}
} // namespace PieceSquareTables
