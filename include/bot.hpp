#ifndef BOT_H
#define BOT_H

#include <string>
#include <vector>

#include "board.hpp"
#include "evaluator.hpp"
#include "move_generator.hpp"
#include "types.hpp"

class Bot
{
  public:
    void load_position(std::string fen);
    Move_t get_move(int max_depth);

  private:
    static constexpr int MATE_SCORE = 100000;
    static constexpr int STALEMATE_SCORE = 0;
    static constexpr int INF = 1000000;

    Board board;
    MoveGenerator move_generator = MoveGenerator(board);
    Evaluator evaluator = Evaluator(board);
    Move_t best_move;
    int search(int depth, int alpha, int beta, bool is_quiescence);
    int get_move_priority(Move_t move);
    void order_moves(std::vector<Move_t> &moves);
    bool is_noisy(Move_t move);
};

#endif
