#include "piece_square_tables.hpp"

#include "color.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "types.hpp"

namespace piece_square_tables
{
int get_piece_score(Piece piece, Square square, bool is_endgame)
{
    const Square table_square =
        piece::get_piece_color(piece) == color::WHITE
            ? square::create_square(square::get_file(square),
                                    square::RANKS - 1 - square::get_rank(square))
            : square;

    switch (piece::get_piece_type(piece))
    {
    case piece::PAWN:
        return is_endgame ? pawn_end[table_square] : pawn_mid[table_square];
    case piece::KNIGHT:
        return is_endgame ? knight_end[table_square] : knight_mid[table_square];
    case piece::BISHOP:
        return is_endgame ? bishop_end[table_square] : bishop_mid[table_square];
    case piece::ROOK:
        return is_endgame ? rook_end[table_square] : rook_mid[table_square];
    case piece::QUEEN:
        return is_endgame ? queen_end[table_square] : queen_mid[table_square];
    case piece::KING:
        return is_endgame ? king_end[table_square] : king_mid[table_square];
    default:
        return 0;
    }
}
} // namespace piece_square_tables
