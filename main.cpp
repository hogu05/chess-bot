#include <iostream>
#include "board.h"
#include "move_generator.h"
#include <vector>
#include "precomputations.h"
#include "move.h"

int main() {
    auto board = Board();
    Precomputations::init();
    board.load_position_from_fen("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ");
    auto move_generator = MoveGenerator(board);
    std::vector<int> moves = move_generator.generate_moves();

    std::cout << "Moves count: " << moves.size() << std::endl;
    /*std::cout << "Moves: ";
    for (int move: moves) {
        std::cout << Move::get_start_square(move) << " -> " << Move::get_target_square(move) << ", " << Move::get_flag(move) << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;*/

    board.print_board();
}

/*
  * 56 57 58 59 60 61 62 63
  * 48 49 50 51 52 53 54 55
  * 40 41 42 43 44 45 46 47
  * 32 33 34 35 36 37 38 39
  * 24 25 26 27 28 29 30 31
  * 16 17 18 19 20 21 22 23
  * 08 09 10 11 12 13 14 15
  * 00 01 02 03 04 05 06 07
*/
