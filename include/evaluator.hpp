#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "board.hpp"

class Evaluator
{
  public:
    explicit Evaluator(Board& board);
    int get_evaluation();

  private:
    Board& board;
};

#endif
