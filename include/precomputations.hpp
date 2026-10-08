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
constexpr std::array<std::array<int, directions::sliding_directions.size()>, square::TOTAL_SQUARES>
compute_squares_to_edge()
{
    std::array<std::array<int, directions::sliding_directions.size()>, square::TOTAL_SQUARES>
        squares_to_edge{};
    for (int rank = 0; rank < square::RANKS; rank++)
    {
        for (int file = 0; file < square::FILES; file++)
        {
            auto& edges = squares_to_edge[square::create_square(file, rank)];
            const int north = square::RANKS - rank - 1;
            const int east = square::FILES - file - 1;
            const int south = rank;
            const int west = file;

            edges[directions::get_direction_index(directions::NORTH)] = north;
            edges[directions::get_direction_index(directions::EAST)] = east;
            edges[directions::get_direction_index(directions::SOUTH)] = south;
            edges[directions::get_direction_index(directions::WEST)] = west;
            edges[directions::get_direction_index(directions::NORTH_EAST)] = std::min(north, east);
            edges[directions::get_direction_index(directions::SOUTH_EAST)] = std::min(south, east);
            edges[directions::get_direction_index(directions::SOUTH_WEST)] = std::min(south, west);
            edges[directions::get_direction_index(directions::NORTH_WEST)] = std::min(north, west);
        }
    }
    return squares_to_edge;
}

constexpr std::array<std::array<int, directions::sliding_directions.size()>, square::TOTAL_SQUARES>
    squares_to_edge = compute_squares_to_edge();

constexpr int get_squares_to_edge(Square square, Direction direction)
{
    return squares_to_edge[square][directions::get_direction_index(direction)];
}

constexpr std::array<std::array<Bitboard, square::TOTAL_SQUARES>, color::COLORS>
compute_pawn_attacks()
{
    std::array<std::array<Bitboard, square::TOTAL_SQUARES>, color::COLORS> pawn_attacks{};
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        for (const Color color : {color::WHITE, color::BLACK})
        {
            Bitboard attacks = 0;
            for (const Direction direction : directions::pawn_attack_directions[color])
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

constexpr std::array<Bitboard, square::TOTAL_SQUARES> compute_knight_moves()
{
    constexpr std::array<std::array<int, 2>, 8> jumps = {
        {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}}};

    std::array<Bitboard, square::TOTAL_SQUARES> knight_moves{};
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        Bitboard moves = 0;
        for (const auto [file_offset, rank_offset] : jumps)
        {
            const int file = square::get_file(square) + file_offset;
            const int rank = square::get_rank(square) + rank_offset;
            if (file >= 0 && file < square::FILES && rank >= 0 && rank < square::RANKS)
            {
                bitboard::set_square(moves, square::create_square(file, rank));
            }
        }
        knight_moves[square] = moves;
    }
    return knight_moves;
}

constexpr std::array<Bitboard, square::TOTAL_SQUARES> compute_king_moves()
{
    std::array<Bitboard, square::TOTAL_SQUARES> king_moves{};
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        Bitboard moves = 0;
        for (const Direction direction : directions::sliding_directions)
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

constexpr std::array<std::array<Bitboard, square::TOTAL_SQUARES>, square::TOTAL_SQUARES>
compute_between()
{
    std::array<std::array<Bitboard, square::TOTAL_SQUARES>, square::TOTAL_SQUARES> table{};
    for (Square from = 0; from < square::TOTAL_SQUARES; from++)
    {
        for (const Direction direction : directions::sliding_directions)
        {
            Bitboard squares = 0;
            for (int i = 1; i <= get_squares_to_edge(from, direction); i++)
            {
                const Square to = from + (direction * i);
                table[from][to] = squares;
                bitboard::set_square(squares, to);
            }
        }
    }
    return table;
}

constexpr std::array<std::array<Bitboard, square::TOTAL_SQUARES>, square::TOTAL_SQUARES>
compute_line()
{
    std::array<std::array<Bitboard, square::TOTAL_SQUARES>, square::TOTAL_SQUARES> table{};
    for (Square from = 0; from < square::TOTAL_SQUARES; from++)
    {
        for (const Direction direction : directions::sliding_directions)
        {
            Bitboard full_line = bitboard::create_bitboard(from);
            for (int i = 1; i <= get_squares_to_edge(from, direction); i++)
            {
                bitboard::set_square(full_line, from + (direction * i));
            }
            for (int i = 1; i <= get_squares_to_edge(from, -direction); i++)
            {
                bitboard::set_square(full_line, from - (direction * i));
            }
            for (int i = 1; i <= get_squares_to_edge(from, direction); i++)
            {
                table[from][from + (direction * i)] = full_line;
            }
        }
    }
    return table;
}

constexpr std::array<std::array<Bitboard, square::TOTAL_SQUARES>, color::COLORS> pawn_attacks =
    compute_pawn_attacks();
constexpr std::array<Bitboard, square::TOTAL_SQUARES> knight_moves = compute_knight_moves();
constexpr std::array<Bitboard, square::TOTAL_SQUARES> king_moves = compute_king_moves();
constexpr std::array<std::array<Bitboard, square::TOTAL_SQUARES>, square::TOTAL_SQUARES> between =
    compute_between();
constexpr std::array<std::array<Bitboard, square::TOTAL_SQUARES>, square::TOTAL_SQUARES> line =
    compute_line();
} // namespace precomputations

#endif
