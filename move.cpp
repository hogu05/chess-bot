#include "move.h"

int Move::get_move(int from_square, int to_square) {
    int move = 0;
    move = move | from_square;
    move = move | (to_square << 5);
    return move;
}