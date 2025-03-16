#include "board.hpp"
#include "move_generator.hpp"
#include "precomputations.hpp"

#include <iostream>
#include <chrono>

int main() {
    auto board = Board();
    Precomputations::init_precomputations();
    board.load_position_from_fen("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    auto move_generator = MoveGenerator(board);

    auto start = std::chrono::high_resolution_clock::now();
    int nodes = move_generator.calculate_nodes(5, true);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Nodes: " << nodes << std::endl;
    std::cout << "Time: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Speed: " << nodes / duration.count() * 1000 << " N/s"<< std::endl;

    return 0;
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
