#include "evaluator.hpp"

#include "piece.hpp"
#include "piece_square_tables.hpp"
#include "square.hpp"
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
    for (const Piece piece : board.get_squares())
    {
        const int piece_value = get_piece_value(piece::get_piece_type(piece));
        total_material += piece_value;
        evaluation +=
            piece::get_piece_color(piece) == board.get_to_move() ? piece_value : -piece_value;
    }

    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        const Piece piece = board.get_squares()[square];
        const int piece_score = piece_square_tables::get_piece_score(
            piece, square, total_material <= ENDGAME_MATERIAL_LIMIT);
        evaluation +=
            piece::get_piece_color(piece) == board.get_to_move() ? piece_score : -piece_score;
    }

    return evaluation;
}
} // namespace evaluator
