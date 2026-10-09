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
            const int score = -search(depth - 1, -INF, -best_score, stop);
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

int Bot::search(int depth, int alpha, int beta, std::atomic<bool>& stop)
{
    if (depth == 0)
    {
        return quiescence_search(alpha, beta, stop);
    }

    searched_nodes++;
    check_deadline(stop);
    if (stop)
    {
        return 0;
    }

    if (is_draw())
    {
        return DRAW_SCORE;
    }

    const int ply = board.get_ply() - search_root_ply;
    Move hash_move = move::NONE_MOVE;
    const TranspositionTable::Entry* entry = transposition_table.probe(board.get_hash());
    if (entry != nullptr)
    {
        hash_move = entry->best_move;
        const int entry_score = get_score_from_table(entry->score, ply);
        if (entry->depth >= depth &&
            (entry->bound == TranspositionTable::Bound::EXACT ||
             (entry->bound == TranspositionTable::Bound::LOWER && entry_score >= beta) ||
             (entry->bound == TranspositionTable::Bound::UPPER && entry_score <= alpha)))
        {
            return std::clamp(entry_score, alpha, beta);
        }
    }

    MoveList moves = move_generator.get_moves();
    order_moves(moves, hash_move);

    Move best_move = move::NONE_MOVE;
    for (const Move move : moves)
    {
        board.make_move(move);
        const int score = -search(depth - 1, -beta, -alpha, stop);
        board.unmake_move(move);

        if (stop)
        {
            return 0;
        }

        if (score >= beta)
        {
            transposition_table.store(board.get_hash(), depth, get_table_score(beta, ply),
                                      TranspositionTable::Bound::LOWER, move);
            return beta;
        }
        if (score > alpha)
        {
            alpha = score;
            best_move = move;
        }
    }

    if (moves.empty())
    {
        return move_generator.is_check() ? -MATE_SCORE + ply : DRAW_SCORE;
    }

    transposition_table.store(board.get_hash(), depth, get_table_score(alpha, ply),
                              best_move != move::NONE_MOVE ? TranspositionTable::Bound::EXACT
                                                           : TranspositionTable::Bound::UPPER,
                              best_move);
    return alpha;
}

int Bot::quiescence_search(int alpha, int beta, std::atomic<bool>& stop)
{
    searched_nodes++;
    check_deadline(stop);
    if (stop)
    {
        return 0;
    }

    if (is_draw())
    {
        return DRAW_SCORE;
    }

    const int static_score = evaluator::evaluate(board);
    if (static_score >= beta)
    {
        return beta;
    }
    alpha = std::max(alpha, static_score);

    MoveList moves = move_generator.get_moves();
    order_moves(moves, move::NONE_MOVE);

    bool found_move = false;
    for (const Move move : moves)
    {
        if (!is_noisy(move))
        {
            continue;
        }

        board.make_move(move);
        const int score = -quiescence_search(-beta, -alpha, stop);
        board.unmake_move(move);

        if (stop)
        {
            return 0;
        }

        found_move = true;
        if (score >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, score);
    }

    if (!found_move)
    {
        return static_score;
    }

    return alpha;
}

void Bot::check_deadline(std::atomic<bool>& stop) const
{
    if (searched_nodes % DEADLINE_CHECK_INTERVAL == 0 &&
        std::chrono::steady_clock::now() >= search_deadline)
    {
        stop = true;
    }
}

bool Bot::is_draw()
{
    return board.is_repetition(search_root_ply) ||
           (board.get_fifty_move_ply() >= FIFTY_MOVE_RULE_PLIES &&
            !(move_generator.get_moves().empty() && move_generator.is_check()));
}

int Bot::get_table_score(int score, int ply)
{
    if (score >= MATE_THRESHOLD)
    {
        return score + ply;
    }
    if (score <= -MATE_THRESHOLD)
    {
        return score - ply;
    }
    return score;
}

int Bot::get_score_from_table(int table_score, int ply)
{
    if (table_score >= MATE_THRESHOLD)
    {
        return table_score - ply;
    }
    if (table_score <= -MATE_THRESHOLD)
    {
        return table_score + ply;
    }
    return table_score;
}

int Bot::get_move_priority(Move move, Move hash_move) const
{
    if (move == hash_move)
    {
        return INF;
    }

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

void Bot::order_moves(MoveList& moves, Move hash_move) const
{
    std::sort(moves.begin(), moves.end(), [this, hash_move](Move a, Move b)
              { return get_move_priority(a, hash_move) > get_move_priority(b, hash_move); });
}

bool Bot::is_noisy(Move move) const
{
    return board.get_captured_piece(move) != piece::NONE || move::is_pawn_promotion(move);
}
