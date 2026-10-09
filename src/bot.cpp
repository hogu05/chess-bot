#include "bot.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <unordered_map>

#include "evaluator.hpp"
#include "move.hpp"
#include "piece.hpp"

Move Bot::go(std::atomic<bool>& stop, std::chrono::steady_clock::time_point deadline, int max_depth)
{
    search_deadline = deadline;
    search_root_ply = board.get_ply();
    searched_nodes = 0;

    MoveList legal_moves = move_generator.get_moves();
    if (legal_moves.empty())
    {
        return move::NONE_MOVE;
    }

    Move best_move = *legal_moves.begin();
    std::unordered_map<Move, int> root_move_scores;

    for (int depth = 1; !stop && depth <= max_depth; depth++)
    {
        int best_score = -INF;
        MoveList root_moves = move_generator.get_moves();

        std::sort(root_moves.begin(), root_moves.end(),
                  [&](Move a, Move b) { return root_move_scores[a] > root_move_scores[b]; });

        for (const Move move : root_moves)
        {
            board.make_move(move);
            const int score = -search(depth - 1, -INF, -best_score, false, stop);
            board.unmake_move(move);

            if (stop)
            {
                break;
            }

            root_move_scores[move] = score;

            if (score > best_score)
            {
                best_score = score;
                best_move = move;
            }
        }
        root_move_scores[best_move] = INF;
    }
    return best_move;
}

Board& Bot::get_board()
{
    return board;
}

MoveGenerator& Bot::get_move_generator()
{
    return move_generator;
}

void Bot::load_position(const std::string& fen)
{
    board.load_position(fen);
}

std::uint64_t Bot::get_searched_nodes() const
{
    return searched_nodes;
}

int Bot::search(int depth, int alpha, int beta, bool is_quiescence, std::atomic<bool>& stop)
{
    searched_nodes++;
    if (searched_nodes % DEADLINE_CHECK_INTERVAL == 0 &&
        std::chrono::steady_clock::now() >= search_deadline)
    {
        stop = true;
    }

    if (stop)
    {
        return 0;
    }

    if (board.is_repetition(search_root_ply))
    {
        return REPETITION_SCORE;
    }

    if (board.get_fifty_move_ply() >= FIFTY_MOVE_RULE_PLIES &&
        !(move_generator.get_moves().empty() && move_generator.is_check()))
    {
        return FIFTY_MOVE_RULE_SCORE;
    }

    if (depth == 0)
    {
        is_quiescence = true;
    }

    int static_score = 0;
    if (is_quiescence)
    {
        static_score = evaluator::evaluate(board);
        if (static_score >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, static_score);
    }

    MoveList moves = move_generator.get_moves();
    order_moves(moves);

    bool found_move = false;
    for (const Move move : moves)
    {
        if (is_quiescence && !is_noisy(move))
        {
            continue;
        }

        board.make_move(move);
        const int score = -search(depth - 1, -beta, -alpha, is_quiescence, stop);
        board.unmake_move(move);

        found_move = true;
        if (score >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, score);
    }

    if (!found_move && is_quiescence)
    {
        return static_score;
    }

    if (!found_move)
    {
        return move_generator.is_check() ? -MATE_SCORE - (depth * 1000) : STALEMATE_SCORE;
    }

    return alpha;
}

int Bot::get_move_priority(Move move) const
{
    int priority = 0;
    const PieceType captured_piece_type = piece::get_piece_type(board.get_captured_piece(move));

    if (captured_piece_type != piece::NONE)
    {
        priority += (10 * evaluator::get_piece_value(captured_piece_type)) -
                    evaluator::get_piece_value(board.get_piece_type(move::get_start_square(move)));
    }

    if (move::is_pawn_promotion(move))
    {
        priority += evaluator::get_piece_value(move::get_pawn_promotion_piece_type(move));
    }

    return priority;
}

void Bot::order_moves(MoveList& moves) const
{
    std::sort(moves.begin(), moves.end(),
              [this](Move a, Move b) { return get_move_priority(a) > get_move_priority(b); });
}

bool Bot::is_noisy(Move move) const
{
    return board.get_captured_piece(move) != piece::NONE || move::is_pawn_promotion(move);
}
