#include "directions.h"

int Directions::get_direction_index(int direction) {
    switch (direction) {
        case NORTH: return 0;
        case EAST: return 1;
        case SOUTH: return 2;
        case WEST: return 3;
        case NORTH_EAST: return 4;
        case NORTH_WEST: return 5;
        case SOUTH_EAST: return 6;
        case SOUTH_WEST: return 7;
        default: return -1;
    }
}
