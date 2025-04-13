#include "evaluator.hpp"
#include "piece.hpp"

Evaluator::Evaluator(Board &board) : board(board){}

int Evaluator::get_evaluation()
{
    int evaluation = 0;
    for (Piece_t piece: board.pieces)
    {
        if (Piece::get_piece_color(piece) == Piece::WHITE)
        {
            evaluation += Piece::get_piece_value(piece);
        }
        else
        {
            evaluation -= Piece::get_piece_value(piece);
        }
    }
    return evaluation;
}