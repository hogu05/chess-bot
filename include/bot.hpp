#ifndef BOT_H
#define BOT_H

#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>

#include "board.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
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

    Board board;
    MoveGenerator move_generator = MoveGenerator(board);
    TranspositionTable transposition_table;
    std::chrono::steady_clock::time_point search_deadline;
    int search_root_ply = 0;
    std::uint64_t searched_nodes = 0;

    int search(int depth, int alpha, int beta, std::atomic<bool>& stop);

    int quiescence_search(int alpha, int beta, std::atomic<bool>& stop);

    void check_deadline(std::atomic<bool>& stop) const;

    bool is_draw();

    static int get_table_score(int score, int ply);

    static int get_score_from_table(int table_score, int ply);

    int get_move_priority(Move move, Move hash_move) const;

    void order_moves(MoveList& moves, Move hash_move) const;

    bool is_noisy(Move move) const;
};

#endif
