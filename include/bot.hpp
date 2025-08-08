#ifndef BOT_H
#define BOT_H

#include <functional>
#include <string>
#include <vector>

#include "board.hpp"
#include "evaluator.hpp"
#include "move_generator.hpp"
#include "types.hpp"

class Bot
{
  public:
    void go(std::function<void(int, Move_t, int)> callback, bool& stop);
    Move_t play(int thinking_time);
    Board& get_board();
    MoveGenerator& get_move_generator();
    void load_position(std::string fen);

  private:
    static constexpr int MATE_SCORE = 100000;
    static constexpr int STALEMATE_SCORE = 0;
    static constexpr int THREEFOLD_REPETITION_SCORE = 0;
    static constexpr int INF = 1000000;

    Board board;
    MoveGenerator move_generator = MoveGenerator(board);
    Evaluator evaluator = Evaluator(board);
    Move_t best_move;
    int search(int depth, int alpha, int beta, bool is_quiescence, bool& stop);
    int get_move_priority(Move_t move);
    void order_moves(std::vector<Move_t>& moves);
    bool is_noisy(Move_t move);
};

#endif
