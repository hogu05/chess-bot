#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <array>

#include "color.hpp"
#include "types.hpp"

namespace directions
{
constexpr Direction NORTH = +8;
constexpr Direction EAST = +1;
constexpr Direction SOUTH = -8;
constexpr Direction WEST = -1;
constexpr Direction NORTH_EAST = NORTH + EAST;
constexpr Direction SOUTH_EAST = SOUTH + EAST;
constexpr Direction SOUTH_WEST = SOUTH + WEST;
constexpr Direction NORTH_WEST = NORTH + WEST;

constexpr std::array<Direction, 8> sliding_directions = {
    NORTH, EAST, SOUTH, WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};
constexpr int LAST_ORTHOGONAL_DIRECTION_INDEX = 3;

constexpr std::array<Direction, 8> knight_directions = {
    NORTH + NORTH + EAST, NORTH + EAST + EAST, SOUTH + EAST + EAST, SOUTH + SOUTH + EAST,
    SOUTH + SOUTH + WEST, SOUTH + WEST + WEST, NORTH + WEST + WEST, NORTH + NORTH + WEST};

constexpr std::array<std::array<Direction, 2>, color::COLORS> pawn_attack_directions = {
    {{NORTH_EAST, NORTH_WEST}, {SOUTH_EAST, SOUTH_WEST}}};

constexpr std::array<Direction, 2> pawn_directions = {NORTH, SOUTH};

constexpr int get_direction_index(Direction direction)
{
    switch (direction)
    {
    case NORTH:
        return 0;
    case EAST:
        return 1;
    case SOUTH:
        return 2;
    case WEST:
        return 3;
    case NORTH_EAST:
        return 4;
    case NORTH_WEST:
        return 5;
    case SOUTH_EAST:
        return 6;
    case SOUTH_WEST:
        return 7;
    default:
        return -1;
    }
}

bool is_diagonal_direction(Direction direction);

bool is_orthogonal_direction(Direction direction);

Direction get_ray_direction(Square start_square, Square end_square);
}; // namespace directions

#endif
