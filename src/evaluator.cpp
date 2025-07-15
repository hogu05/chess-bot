#include "evaluator.hpp"

#include "piece.hpp"
#include "position_info.hpp"
#include "types.hpp"

Evaluator::Evaluator(Board &board) : board(board)
{
}

int Evaluator::get_evaluation()
{
    int evaluation = 0;
    for (Piece_t piece : board.pieces)
    {
        int piece_value = Piece::get_piece_value(Piece::get_piece_type(piece));
        if (Piece::get_piece_color(piece) == PositionInfo::get_to_move(board.position_info))
        {
            evaluation += piece_value;
        }
        else
        {
            evaluation -= piece_value;
        }
    }
    return evaluation;
}
