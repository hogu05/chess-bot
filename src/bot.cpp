#include "bot.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <unordered_map>

#include "evaluator.hpp"
#include "move.hpp"
#include "move_list.hpp"
#include "piece.hpp"

Move Bot::go(std::atomic<bool>& stop, std::chrono::steady_clock::time_point deadline, int max_depth)
{
    search_deadline = deadline;
    search_root_ply = board.get_ply();
    searched_nodes = 0;
    killer_moves = {};
    history_scores = {};

    MoveList legal_moves = move_generator.get_moves();
    if (legal_moves.empty())
    {
        return move::NONE_MOVE;
    }

    Move best_move = *legal_moves.begin();
    std::unordered_map<Move, int> root_move_scores;

    for (int depth = 1; !stop && depth <= std::min(max_depth, MAX_DEPTH); depth++)
    {
        int best_score = -INF;
        MoveList root_moves = move_generator.get_moves();

        std::sort(root_moves.begin(), root_moves.end(),
                  [&](Move a, Move b) { return root_move_scores[a] > root_move_scores[b]; });

        for (const Move move : root_moves)
        {
            const int score =
                search_move(move, move == root_moves[0], depth, 0, best_score, INF, stop);

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
    if (depth <= 0)
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
    const bool is_check = move_generator.is_check();
    if (moves.empty())
    {
        return is_check ? -MATE_SCORE + ply : DRAW_SCORE;
    }

    if (depth >= NULL_MOVE_MIN_DEPTH && beta - alpha == 1 && !is_check &&
        board.has_non_pawn_material(board.get_to_move()))
    {
        board.make_null_move();
        const int score = -search(depth - 1 - NULL_MOVE_REDUCTION, -beta, -beta + 1, stop);
        board.unmake_null_move();

        if (score >= beta)
        {
            return beta;
        }
    }

    order_moves(moves, hash_move, killer_moves[ply]);

    Move best_move = move::NONE_MOVE;
    for (int i = 0; i < moves.size(); i++)
    {
        const Move move = moves[i];
        const int score =
            search_move(move, i == 0, depth, get_late_move_reduction(move, i, depth, is_check),
                        alpha, beta, stop);

        if (stop)
        {
            return 0;
        }

        if (score >= beta)
        {
            if (!is_noisy(move))
            {
                update_killer_moves(move, ply);
                update_history_score(move, depth);
            }
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

    transposition_table.store(board.get_hash(), depth, get_table_score(alpha, ply),
                              best_move != move::NONE_MOVE ? TranspositionTable::Bound::EXACT
                                                           : TranspositionTable::Bound::UPPER,
                              best_move);
    return alpha;
}

int Bot::search_move(Move move, bool is_first_move, int depth, int reduction, int alpha, int beta,
                     std::atomic<bool>& stop)
{
    board.make_move(move);
    int score = 0;
    if (is_first_move)
    {
        score = -search(depth - 1, -beta, -alpha, stop);
    }
    else
    {
        score = -search(depth - 1 - reduction, -alpha - 1, -alpha, stop);
        if (score > alpha && reduction > 0)
        {
            score = -search(depth - 1, -alpha - 1, -alpha, stop);
        }
        if (score > alpha && score < beta)
        {
            score = -search(depth - 1, -beta, -alpha, stop);
        }
    }
    board.unmake_move(move);
    return score;
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

    MoveList noisy_moves;
    for (const Move move : move_generator.get_moves())
    {
        if (is_noisy(move))
        {
            noisy_moves.push_back(move);
        }
    }
    order_moves(noisy_moves, move::NONE_MOVE, {});

    for (const Move move : noisy_moves)
    {
        board.make_move(move);
        const int score = -quiescence_search(-beta, -alpha, stop);
        board.unmake_move(move);

        if (stop)
        {
            return 0;
        }

        if (score >= beta)
        {
            return beta;
        }
        alpha = std::max(alpha, score);
    }

    if (noisy_moves.empty())
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

int Bot::get_late_move_reduction(Move move, int move_index, int depth, bool is_check) const
{
    if (depth >= LATE_MOVE_MIN_DEPTH && move_index >= LATE_MOVE_MIN_INDEX && !is_check &&
        !is_noisy(move))
    {
        return LATE_MOVE_REDUCTION;
    }
    return 0;
}

void Bot::update_killer_moves(Move move, int ply)
{
    if (killer_moves[ply][0] != move)
    {
        killer_moves[ply][1] = killer_moves[ply][0];
        killer_moves[ply][0] = move;
    }
}

void Bot::update_history_score(Move move, int depth)
{
    const int bonus = std::min(depth * depth, MAX_HISTORY_SCORE);
    int& history_score = history_scores[board.get_to_move()][move::get_start_square(move)]
                                       [move::get_target_square(move)];
    history_score += bonus - (history_score * bonus / MAX_HISTORY_SCORE);
}

int Bot::get_move_priority(Move move, Move hash_move,
                           const std::array<Move, KILLER_MOVES_PER_PLY>& killers) const
{
    if (move == hash_move)
    {
        return HASH_MOVE_PRIORITY;
    }

    if (!is_noisy(move))
    {
        if (move == killers[0])
        {
            return FIRST_KILLER_MOVE_PRIORITY;
        }
        if (move == killers[1])
        {
            return SECOND_KILLER_MOVE_PRIORITY;
        }
        return history_scores[board.get_to_move()][move::get_start_square(move)]
                             [move::get_target_square(move)];
    }

    int priority = NOISY_MOVE_PRIORITY;
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

void Bot::order_moves(MoveList& moves, Move hash_move,
                      const std::array<Move, KILLER_MOVES_PER_PLY>& killers) const
{
    std::array<int, MoveList::MAX_MOVES> priorities{};
    for (int i = 0; i < moves.size(); i++)
    {
        const Move move = moves[i];
        const int priority = get_move_priority(move, hash_move, killers);

        int j = i;
        while (j > 0 && priorities[j - 1] < priority)
        {
            priorities[j] = priorities[j - 1];
            moves[j] = moves[j - 1];
            j--;
        }
        priorities[j] = priority;
        moves[j] = move;
    }
}

bool Bot::is_noisy(Move move) const
{
    return board.get_captured_piece(move) != piece::NONE || move::is_pawn_promotion(move);
}
