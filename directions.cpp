#include "directions.h"
#include "board.h"

int Directions::get_direction_index(int direction) {
    switch (direction) {
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

bool Directions::is_diagonal_direction(int direction) {
    if (get_direction_index(direction) >= 4) {
        return true;
    }
    return false;
}

bool Directions::is_orthogonal_direction(int direction) {
    if (get_direction_index(direction) <= 3) {
        return true;
    }
    return false;
}

int Directions::get_ray_direction(int start_square, int end_square) {
    int file_diff = Board::get_file(end_square) - Board::get_file(start_square);
    int rank_diff = Board::get_rank(end_square) - Board::get_rank(start_square);

    if (rank_diff > 0) {
        if (file_diff > 0) {
            return NORTH_EAST;
        }
        if (file_diff < 0) {
            return NORTH_WEST;
        }
        return NORTH;
    }

    if (rank_diff == 0) {
        if (file_diff > 0) {
            return EAST;
        }
        return WEST;
    }

    if (file_diff > 0) {
        return SOUTH_EAST;
    }
    if (file_diff < 0) {
        return SOUTH_WEST;
    }
    return SOUTH;
}

