#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <array>

#include "types.hpp"

namespace Directions
{
constexpr Direction_t NORTH = +8;
constexpr Direction_t EAST = +1;
constexpr Direction_t SOUTH = -8;
constexpr Direction_t WEST = -1;
constexpr Direction_t NORTH_EAST = NORTH + EAST;
constexpr Direction_t SOUTH_EAST = SOUTH + EAST;
constexpr Direction_t SOUTH_WEST = SOUTH + WEST;
constexpr Direction_t NORTH_WEST = NORTH + WEST;

constexpr std::array<Direction_t, 8> sliding_directions = {
    NORTH, EAST, SOUTH, WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST, NORTH_WEST};

constexpr std::array<Direction_t, 8> knight_directions = {
    NORTH + NORTH + EAST, NORTH + EAST + EAST, SOUTH + EAST + EAST, SOUTH + SOUTH + EAST,
    SOUTH + SOUTH + WEST, SOUTH + WEST + WEST, NORTH + WEST + WEST, NORTH + NORTH + WEST};

constexpr std::array<std::array<Direction_t, 2>, 2> pawn_attack_directions = {
    {{NORTH_EAST, NORTH_WEST}, {SOUTH_EAST, SOUTH_WEST}}};

constexpr std::array<Direction_t, 2> pawn_directions = {NORTH, SOUTH};

int get_direction_index(Direction_t direction);

bool is_diagonal_direction(Direction_t direction);

bool is_orthogonal_direction(Direction_t direction);

Direction_t get_ray_direction(Square_t start_square, Square_t end_square);
}; // namespace Directions

#endif
