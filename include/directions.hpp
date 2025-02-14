#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <array>

namespace Directions {
    const int NORTH = +8;
    const int EAST = +1;
    const int SOUTH = -8;
    const int WEST = -1;
    const int NORTH_EAST = NORTH + EAST;
    const int SOUTH_EAST = SOUTH + EAST;
    const int SOUTH_WEST = SOUTH + WEST;
    const int NORTH_WEST = NORTH + WEST;

    constexpr std::array<int, 8> sliding_directions = { // TODO: Maybe rename later
        NORTH,
        EAST,
        SOUTH,
        WEST,
        NORTH_EAST,
        SOUTH_EAST,
        SOUTH_WEST,
        NORTH_WEST
    };

    constexpr std::array<int, 8> knight_directions = {
        NORTH + NORTH + EAST,
        NORTH + EAST + EAST,
        SOUTH + EAST + EAST,
        SOUTH + SOUTH + EAST,
        SOUTH + SOUTH + WEST,
        SOUTH + WEST + WEST,
        NORTH + WEST + WEST,
        NORTH + NORTH + WEST
    };

    constexpr std::array<std::array<int, 2>, 2> pawn_attack_directions = {
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


    constexpr std::array<int, 2> pawn_directions = {
        NORTH,
        SOUTH
    };

    int get_direction_index(int direction);
    bool is_diagonal_direction(int direction);
    bool is_orthogonal_direction(int direction);

    int get_ray_direction(int start_square, int end_square);
};



#endif
