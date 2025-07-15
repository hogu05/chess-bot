#include "bot.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "move.hpp"
#include "piece.hpp"

void Bot::load_position(std::string fen)
{
    board.load_position(std::move(fen));
}

Move_t Bot::get_move(int max_depth)
{
    Move_t best_move;
    std::unordered_map<Move_t, int> root_move_scores;

    for (int depth = 1; depth <= max_depth; depth++)
    {
        int best_score = -INF;
        std::vector<Move_t> root_moves = move_generator.get_moves();

        std::sort(root_moves.begin(), root_moves.end(),
                  [&](Move_t a, Move_t b) { return root_move_scores[a] > root_move_scores[b]; });

        for (Move_t move : root_moves)
        {
            board.make_move(move);
            int score = -search(depth - 1, -INF, INF, false);
            board.unmake_move(move);

            root_move_scores[move] = score;

            if (score > best_score)
            {
                best_score = score;
                best_move = move;
            }
        }

        std::cout << "Depth " << depth << ": Best = " << Move::get_move_notation(best_move)
                  << ", Eval = " << best_score << "\n";
    }

    return best_move;
}

int Bot::search(int depth, int alpha, int beta, bool is_quiescence)
{
    if (depth == 0)
    {
        is_quiescence = true;
    }

    int static_score = evaluator.get_evaluation();
    if (is_quiescence)
    {
        if (static_score >= beta)
            return beta;
        if (alpha < static_score)
            alpha = static_score;
    }

    std::vector<Move_t> moves = move_generator.get_moves();
    order_moves(moves);

    bool found_move = false;
    for (Move_t move : moves)
    {
        if (is_quiescence && !is_noisy(move))
        {
            continue;
        }

        board.make_move(move);
        int score = -search(depth - 1, -beta, -alpha, is_quiescence);
        board.unmake_move(move);

        found_move = true;
        if (score >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, score);
    }

    if (!found_move && is_quiescence)
        return static_score;

    if (!found_move)
    {
        return move_generator.is_check() ? -MATE_SCORE : 0;
    }

    return alpha;
}

int Bot::get_move_priority(Move_t move)
{
    int priority = 0;
    PieceType_t moved_piece_type =
        Piece::get_piece_type(board.pieces[Move::get_start_square(move)]);
    PieceType_t captured_piece_type = Piece::get_piece_type(board.get_captured_piece(move));

    if (captured_piece_type != Piece::NONE)
    {
        priority += 10 * Piece::get_piece_value(captured_piece_type) -
                    Piece::get_piece_value(moved_piece_type);
    }

    priority += Piece::get_piece_value(Move::get_pawn_promotion_piece_type(move));

    return priority;
}

void Bot::order_moves(std::vector<Move_t> &moves)
{
    std::sort(moves.begin(), moves.end(), [this](const Move_t &move_1, const Move_t &move_2)
              { return get_move_priority(move_1) > get_move_priority(move_2); });
}

bool Bot::is_noisy(Move_t move)
{
    return board.get_captured_piece(move) != Piece::NONE ||
           Move::get_pawn_promotion_piece_type(move) != Piece::NONE;
}
