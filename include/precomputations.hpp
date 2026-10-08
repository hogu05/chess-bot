#ifndef PRECOMPUTATIONS_H
#define PRECOMPUTATIONS_H

#include <algorithm>
#include <array>

#include "bitboard.hpp"
#include "color.hpp"
#include "directions.hpp"
#include "square.hpp"
#include "types.hpp"

namespace precomputations
{
using SquaresToEdge =
    std::array<std::array<int, directions::sliding_directions.size()>, square::TOTAL_SQUARES>;

constexpr SquaresToEdge compute_squares_to_edge()
{
    SquaresToEdge squares_to_edge{};
    for (int rank = 0; rank < square::RANKS; rank++)
    {
        for (int file = 0; file < square::FILES; file++)
        {
            auto& edges = squares_to_edge[square::create_square(file, rank)];
            int north = square::RANKS - rank - 1;
            int east = square::FILES - file - 1;
            int south = rank;
            int west = file;

            edges[directions::get_direction_index(directions::NORTH)] = north;
            edges[directions::get_direction_index(directions::EAST)] = east;
            edges[directions::get_direction_index(directions::SOUTH)] = south;
            edges[directions::get_direction_index(directions::WEST)] = west;
            edges[directions::get_direction_index(directions::NORTH_EAST)] = std::min(north, east);
            edges[directions::get_direction_index(directions::NORTH_WEST)] = std::min(north, west);
            edges[directions::get_direction_index(directions::SOUTH_EAST)] = std::min(south, east);
            edges[directions::get_direction_index(directions::SOUTH_WEST)] = std::min(south, west);
        }
    }
    return squares_to_edge;
}

inline constexpr SquaresToEdge squares_to_edge = compute_squares_to_edge();

constexpr int get_squares_to_edge(Square square, Direction direction)
{
    return squares_to_edge[square][directions::get_direction_index(direction)];
}

using SquareTable = std::array<Bitboard, square::TOTAL_SQUARES>;

constexpr std::array<SquareTable, color::COLORS> compute_pawn_attacks()
{
    std::array<SquareTable, color::COLORS> pawn_attacks{};
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        for (Color color : {color::WHITE, color::BLACK})
        {
            Bitboard attacks = 0;
            for (Direction direction : directions::pawn_attack_directions[color])
            {
                if (get_squares_to_edge(square, direction) > 0)
                {
                    bitboard::set_square(attacks, square + direction);
                }
            }
            pawn_attacks[color][square] = attacks;
        }
    }
    return pawn_attacks;
}

constexpr SquareTable compute_knight_moves()
{
    constexpr std::array<std::array<int, 2>, 8> jumps = {
        {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}}};

    SquareTable knight_moves{};
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        Bitboard moves = 0;
        for (auto [file_offset, rank_offset] : jumps)
        {
            int file = square::get_file(square) + file_offset;
            int rank = square::get_rank(square) + rank_offset;
            if (file >= 0 && file < square::FILES && rank >= 0 && rank < square::RANKS)
            {
                bitboard::set_square(moves, square::create_square(file, rank));
            }
        }
        knight_moves[square] = moves;
    }
    return knight_moves;
}

constexpr SquareTable compute_king_moves()
{
    SquareTable king_moves{};
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        Bitboard moves = 0;
        for (Direction direction : directions::sliding_directions)
        {
            if (get_squares_to_edge(square, direction) > 0)
            {
                bitboard::set_square(moves, square + direction);
            }
        }
        king_moves[square] = moves;
    }
    return king_moves;
}

inline constexpr std::array<SquareTable, color::COLORS> pawn_attacks =
    compute_pawn_attacks();
inline constexpr SquareTable knight_moves = compute_knight_moves();
inline constexpr SquareTable king_moves = compute_king_moves();
} // namespace precomputations

#endif
