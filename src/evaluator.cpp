#include "evaluator.hpp"

#include "piece.hpp"
#include "piece_square_tables.hpp"
#include "position_info.hpp"
#include "types.hpp"

Evaluator::Evaluator(Board& board) : board(board)
{
}

int Evaluator::get_evaluation()
{
    int evaluation = 0;
    int total_material = 0;
    for (Piece_t piece : board.get_pieces())
    {
        int piece_value = Piece::get_piece_value(Piece::get_piece_type(piece));
        total_material += piece_value;
        if (Piece::get_piece_color(piece) == PositionInfo::get_to_move(board.get_position_info()))
        {
            evaluation += piece_value;
        }
        else
        {
            evaluation -= piece_value;
        }
    }

    bool is_endgame = total_material <= ENDGAME_MATERIAL_LIMIT;
    for (Square_t square = 0; square < Board::TOTAL_SQUARES; square++)
    {
        Piece_t piece = board.get_pieces()[square];
        PieceSquareTables::get_piece_score(piece, square, is_endgame);
        if (Piece::get_piece_color(piece) == PositionInfo::get_to_move(board.get_position_info()))
        {
            evaluation += PieceSquareTables::get_piece_score(piece, square, is_endgame);
        }
        else
        {
            evaluation -= PieceSquareTables::get_piece_score(piece, square, is_endgame);
        }
    }

    return evaluation;
}
