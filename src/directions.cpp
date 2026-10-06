#include "directions.hpp"

#include "square.hpp"
namespace directions
{
bool is_diagonal_direction(Direction direction)
{
    return get_direction_index(direction) > LAST_ORTHOGONAL_DIRECTION_INDEX;
}

bool is_orthogonal_direction(Direction direction)
{
    return get_direction_index(direction) <= LAST_ORTHOGONAL_DIRECTION_INDEX;
}

Direction get_ray_direction(Square start_square, Square end_square)
{
    int file_diff = square::get_file(end_square) - square::get_file(start_square);
    int rank_diff = square::get_rank(end_square) - square::get_rank(start_square);

    if (rank_diff > 0)
    {
        if (file_diff > 0)
        {
            return NORTH_EAST;
        }
        if (file_diff < 0)
        {
            return NORTH_WEST;
        }
        return NORTH;
    }
    if (rank_diff == 0)
    {
        if (file_diff > 0)
        {
            return EAST;
        }
        return WEST;
    }
    if (file_diff > 0)
    {
        return SOUTH_EAST;
    }
    if (file_diff < 0)
    {
        return SOUTH_WEST;
    }
    return SOUTH;
}
} // namespace directions
