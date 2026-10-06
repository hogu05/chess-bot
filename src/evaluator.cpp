#include "evaluator.hpp"

#include "piece.hpp"
#include "piece_square_tables.hpp"
#include "position_info.hpp"
#include "types.hpp"

namespace evaluator
{
namespace
{
constexpr int ENDGAME_MATERIAL_LIMIT = 4000;
} // namespace

int evaluate(const Board& board)
{
    int evaluation = 0;
    int total_material = 0;
    for (Piece piece : board.get_pieces())
    {
        int piece_value = evaluator::get_piece_value(piece::get_piece_type(piece));
        total_material += piece_value;
        if (piece::get_piece_color(piece) == position_info::get_to_move(board.get_position_info()))
        {
            evaluation += piece_value;
        }
        else
        {
            evaluation -= piece_value;
        }
    }

    bool is_endgame = total_material <= ENDGAME_MATERIAL_LIMIT;
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        Piece piece = board.get_pieces()[square];
        piece_square_tables::get_piece_score(piece, square, is_endgame);
        if (piece::get_piece_color(piece) == position_info::get_to_move(board.get_position_info()))
        {
            evaluation += piece_square_tables::get_piece_score(piece, square, is_endgame);
        }
        else
        {
            evaluation -= piece_square_tables::get_piece_score(piece, square, is_endgame);
        }
    }

    return evaluation;
}
} // namespace evaluator
