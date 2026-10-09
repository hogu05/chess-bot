#ifndef BOT_H
#define BOT_H

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>

#include "board.hpp"
#include "color.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "square.hpp"
#include "transposition_table.hpp"
#include "types.hpp"

class Bot
{
  public:
    static constexpr int MAX_DEPTH = 256;

    Move go(std::atomic<bool>& stop, std::chrono::steady_clock::time_point deadline, int max_depth);

    Board& get_board();

    MoveGenerator& get_move_generator();

    void load_position(const std::string& fen);

    std::uint64_t get_searched_nodes() const;

  private:
    static constexpr int MATE_SCORE = 100000;
    static constexpr int MATE_THRESHOLD = MATE_SCORE - MAX_DEPTH;
    static constexpr int DRAW_SCORE = 0;
    static constexpr int FIFTY_MOVE_RULE_PLIES = 100;
    static constexpr int INF = 1000000;
    static constexpr int DEADLINE_CHECK_INTERVAL = 2048;
    static constexpr int KILLER_MOVES_PER_PLY = 2;
    static constexpr int HASH_MOVE_PRIORITY = 2000000;
    static constexpr int NOISY_MOVE_PRIORITY = 1000000;
    static constexpr int FIRST_KILLER_MOVE_PRIORITY = 900000;
    static constexpr int SECOND_KILLER_MOVE_PRIORITY = 800000;
    static constexpr int MAX_HISTORY_SCORE = 16384;
    static constexpr int NULL_MOVE_MIN_DEPTH = 3;
    static constexpr int NULL_MOVE_REDUCTION = 2;
    static constexpr int LATE_MOVE_MIN_DEPTH = 3;
    static constexpr int LATE_MOVE_MIN_INDEX = 3;
    static constexpr int LATE_MOVE_REDUCTION = 1;
    static constexpr int REVERSE_FUTILITY_MAX_DEPTH = 6;
    static constexpr int REVERSE_FUTILITY_MARGIN = 100;
    static constexpr int FUTILITY_MAX_DEPTH = 2;
    static constexpr int FUTILITY_MARGIN = 150;

    Board board;
    MoveGenerator move_generator = MoveGenerator(board);
    TranspositionTable transposition_table;
    std::array<std::array<Move, KILLER_MOVES_PER_PLY>, MAX_DEPTH> killer_moves{};
    std::array<std::array<std::array<int, square::TOTAL_SQUARES>, square::TOTAL_SQUARES>,
               color::COLORS>
        history_scores{};
    std::chrono::steady_clock::time_point search_deadline;
    int search_root_ply = 0;
    std::uint64_t searched_nodes = 0;

    int search(int depth, int alpha, int beta, std::atomic<bool>& stop);

    int search_move(Move move, bool is_first_move, int depth, int reduction, int alpha, int beta,
                    std::atomic<bool>& stop);

    int quiescence_search(int alpha, int beta, std::atomic<bool>& stop);

    void check_deadline(std::atomic<bool>& stop) const;

    bool is_draw();

    static int get_table_score(int score, int ply);

    static int get_score_from_table(int table_score, int ply);

    int get_late_move_reduction(Move move, int move_index, int depth, bool is_check) const;

    void update_killer_moves(Move move, int ply);

    void update_history_score(Move move, int depth);

    int get_move_priority(Move move, Move hash_move,
                          const std::array<Move, KILLER_MOVES_PER_PLY>& killers) const;

    void order_moves(MoveList& moves, Move hash_move,
                     const std::array<Move, KILLER_MOVES_PER_PLY>& killers) const;

    bool is_noisy(Move move) const;
};

#endif
