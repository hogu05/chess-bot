#include "bot.hpp"
#include "precomputations.hpp"
int main()
{
    Precomputations::init_precomputations();

    Bot bot;
    bot.load_position("rnb1kbnr/1pp1pppp/p1qp4/8/P3P3/2N2N2/1PPP1PPP/R1BQKB1R w KQkq - 0 1");
    Move_t best_move = bot.get_move(8);
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
