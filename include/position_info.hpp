#ifndef POSITION_INFO_H
#define POSITION_INFO_H

#include <array>
#include "piece.hpp"

// TODO: change to namespace and one simple int
class PositionInfo {
public:
    int to_move = Piece::WHITE;
    int captured_piece = Piece::NONE;
    int en_passant = -1;
    std::array<bool, 2> can_short_castle = {false, false};
    std::array<bool, 2> can_long_castle = {false, false};
    int fifty_move_ply = 0;
};



#endif
