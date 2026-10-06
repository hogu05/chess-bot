#include "bot.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>
#include <unordered_map>
#include <vector>

#include "evaluator.hpp"
#include "move.hpp"
#include "piece.hpp"

void Bot::go(const std::function<void(int, Move, int)>& callback, bool& stop)
{
    Move best_move = 0;
    std::unordered_map<Move, int> root_move_scores;

    for (int depth = 1; !stop; depth++)
    {
        int best_score = -INF;
        std::vector<Move> root_moves = move_generator.get_moves();

        std::sort(root_moves.begin(), root_moves.end(),
                  [&](Move a, Move b) { return root_move_scores[a] > root_move_scores[b]; });

        for (Move move : root_moves)
        {
            if (stop)
            {
                break;
            }

            board.make_move(move);
            int score = -search(depth - 1, -INF, INF, false, stop);
            board.unmake_move(move);

            root_move_scores[move] = score;

            if (score > best_score)
            {
                best_score = score;
                best_move = move;
            }
        }

        if (!stop)
        {
            callback(depth, best_move, best_score);
        }
    }
}

Move Bot::play(int thinking_time)
{
    Move best_move = 0;
    bool stop = false;

    auto depth_callback = [this, &best_move](int depth, const Move& move, int eval)
    { best_move = move; };

    std::thread analysis_thread([this, &depth_callback, &stop] { go(depth_callback, stop); });

    std::this_thread::sleep_for(std::chrono::milliseconds(thinking_time));

    stop = true;
    analysis_thread.join();
    return best_move;
}

int Bot::search(int depth, int alpha, int beta, bool is_quiescence, bool& stop)
{
    if (stop)
    {
        return 0;
    }

    if (board.is_threefold_repetition())
    {
        return THREEFOLD_REPETITION_SCORE;
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

    std::vector<Move> moves = move_generator.get_moves();
    order_moves(moves);

    bool found_move = false;
    for (Move move : moves)
    {
        if (is_quiescence && !is_noisy(move))
        {
            continue;
        }

        board.make_move(move);
        int score = -search(depth - 1, -beta, -alpha, is_quiescence, stop);
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
    PieceType moved_piece_type =
        piece::get_piece_type(board.get_pieces()[move::get_start_square(move)]);
    PieceType captured_piece_type = piece::get_piece_type(board.get_captured_piece(move));

    if (captured_piece_type != piece::NONE)
    {
        priority += (10 * evaluator::get_piece_value(captured_piece_type)) -
                    evaluator::get_piece_value(moved_piece_type);
    }

    if (move::is_pawn_promotion(move))
    {
        priority += evaluator::get_piece_value(move::get_pawn_promotion_piece_type(move));
    }

    return priority;
}

void Bot::order_moves(std::vector<Move>& moves) const
{
    std::sort(moves.begin(), moves.end(), [this](const Move& move_1, const Move& move_2)
              { return get_move_priority(move_1) > get_move_priority(move_2); });
}

bool Bot::is_noisy(Move move) const
{
    return (board.get_captured_piece(move) != piece::NONE) || move::is_pawn_promotion(move);
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
