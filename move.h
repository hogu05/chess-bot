#ifndef MOVE_H
#define MOVE_H



class Move {
public:
    static const int FROM_SQUARE_MASK = 0b11111;
    static const int TO_SQUARE_MASK = 0b1111100000;

    static int get_move(int from_square, int to_square);
};



#endif
