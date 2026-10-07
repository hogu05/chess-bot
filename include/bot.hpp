#ifndef BOT_H
#define BOT_H

#include <atomic>
#include <chrono>
#include <string>

#include "board.hpp"
#include "move_generator.hpp"
#include "move_list.hpp"
#include "types.hpp"

class Bot
{
  public:
    Move go(std::atomic<bool>& stop, std::chrono::steady_clock::time_point deadline);
    Board& get_board();
    MoveGenerator& get_move_generator();
    void load_position(const std::string& fen);

  private:
    static constexpr int MATE_SCORE = 100000;
    static constexpr int STALEMATE_SCORE = 0;
    static constexpr int THREEFOLD_REPETITION_SCORE = 0;
    static constexpr int INF = 1000000;
    static constexpr int DEADLINE_CHECK_INTERVAL = 2048;

    Board board;
    MoveGenerator move_generator = MoveGenerator(board);
    std::chrono::steady_clock::time_point search_deadline;
    std::uint64_t searched_nodes = 0;
    int search(int depth, int alpha, int beta, bool is_quiescence, std::atomic<bool>& stop);
    int get_move_priority(Move move) const;
    void order_moves(MoveList& moves) const;
    bool is_noisy(Move move) const;
};

#endif
