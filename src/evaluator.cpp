#include "evaluator.hpp"

#include <algorithm>
#include <array>

#include "piece.hpp"
#include "piece_square_tables.hpp"
#include "square.hpp"
#include "types.hpp"

namespace evaluator
{
namespace
{
constexpr std::array<int, piece::PIECE_TYPE_COUNT + 1> MIDDLEGAME_PIECE_VALUES = {
    0, 82, 337, 365, 477, 1025, 0};
constexpr std::array<int, piece::PIECE_TYPE_COUNT + 1> ENDGAME_PIECE_VALUES = {0,   94,  281, 297,
                                                                               512, 936, 0};
constexpr std::array<int, piece::PIECE_TYPE_COUNT + 1> PHASE_INCREMENTS = {0, 0, 1, 1, 2, 4, 0};
constexpr int MAX_PHASE = 24;
} // namespace

int evaluate(const Board& board)
{
    int middlegame_evaluation = 0;
    int endgame_evaluation = 0;
    int phase = 0;
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        const Piece piece = board.get_squares()[square];
        const PieceType piece_type = piece::get_piece_type(piece);
        const int middlegame_score = MIDDLEGAME_PIECE_VALUES[piece_type] +
                                     piece_square_tables::get_piece_score(piece, square, false);
        const int endgame_score = ENDGAME_PIECE_VALUES[piece_type] +
                                  piece_square_tables::get_piece_score(piece, square, true);

        if (piece::get_piece_color(piece) == board.get_to_move())
        {
            middlegame_evaluation += middlegame_score;
            endgame_evaluation += endgame_score;
        }
        else
        {
            middlegame_evaluation -= middlegame_score;
            endgame_evaluation -= endgame_score;
        }
        phase += PHASE_INCREMENTS[piece_type];
    }

    const int middlegame_phase = std::min(phase, MAX_PHASE);
    return ((middlegame_evaluation * middlegame_phase) +
            (endgame_evaluation * (MAX_PHASE - middlegame_phase))) /
           MAX_PHASE;
}
} // namespace evaluator
