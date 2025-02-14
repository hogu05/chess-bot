#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "board.hpp"


class Evaluator {
public:
    Evaluator(Board* board);
    int evaluate();
private:
    Board* board;
};



#endif
