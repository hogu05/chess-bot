#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <array>
#include <unordered_map>

class Directions {
public:
    static const int NORTH = +8;
    static const int EAST = +1;
    static const int SOUTH = -8;
    static const int WEST = -1;
    static const int NORTH_EAST = NORTH + EAST;
    static const int SOUTH_EAST = SOUTH + EAST;
    static const int SOUTH_WEST = SOUTH + WEST;
    static const int NORTH_WEST = NORTH + WEST;

    static constexpr std::array<int, 8> sliding_directions = { // TODO: Maybe rename later
        NORTH,
        EAST,
        SOUTH,
        WEST,
        NORTH_EAST,
        SOUTH_EAST,
        SOUTH_WEST,
        NORTH_WEST
    };

    static constexpr std::array<int, 8> knight_directions = {
        NORTH + NORTH + EAST,
        NORTH + EAST + EAST,
        SOUTH + EAST + EAST,
        SOUTH + SOUTH + EAST,
        SOUTH + SOUTH + WEST,
        SOUTH + WEST + WEST,
        NORTH + WEST + WEST,
        NORTH + NORTH + WEST
    };

    static constexpr std::array<std::array<int, 2>, 2> pawn_attack_directions = {
        {
            // White
            {
                NORTH_EAST,
                NORTH_WEST
            },
            // Black
            {
                SOUTH_EAST,
                SOUTH_WEST
            }
        }
    };


    static constexpr std::array<int, 2> pawn_directions = {
        NORTH,
        SOUTH
    };


    static int get_direction_index(int direction); // TODO: idk
};



#endif
