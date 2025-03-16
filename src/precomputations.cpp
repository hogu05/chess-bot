#include "precomputations.hpp"
#include "board.hpp"
#include "directions.hpp"
#include "bitboard.hpp"
#include "piece.hpp"

#include <iostream>

void Precomputations::init_precomputations() {
    calculate_squares_to_edge();
    calculate_pawn_attacks();
    calculate_knight_moves();
    calculate_king_moves();
}

std::array<std::array<int, 8>, Board::TOTAL_SQUARES> Precomputations::squares_to_edge;
void Precomputations::calculate_squares_to_edge() {
    for (int rank = 0; rank < Board::RANKS; rank++) {
        for (int file = 0; file < Board::FILES; file++) {
            int square = Board::get_square(file, rank);

            squares_to_edge[square][Directions::get_direction_index(Directions::NORTH)] = Board::RANKS - rank - 1;
            squares_to_edge[square][Directions::get_direction_index(Directions::EAST)] = Board::FILES - file - 1;
            squares_to_edge[square][Directions::get_direction_index(Directions::SOUTH)] = rank;
            squares_to_edge[square][Directions::get_direction_index(Directions::WEST)] = file;
            squares_to_edge[square][Directions::get_direction_index(Directions::NORTH_EAST)] =
                std::min(Board::RANKS - rank - 1, Board::FILES - file - 1);
            squares_to_edge[square][Directions::get_direction_index(Directions::NORTH_WEST)] =
                std::min(Board::RANKS - rank - 1, file);
            squares_to_edge[square][Directions::get_direction_index(Directions::SOUTH_EAST)] =
                std::min(rank, Board::FILES - file - 1);
            squares_to_edge[square][Directions::get_direction_index(Directions::SOUTH_WEST)] =
                std::min(rank, file);
        }
    }
}

int Precomputations::get_squares_to_edge(Square_t square, Direction_t direction) {
    return squares_to_edge[square][Directions::get_direction_index(direction)];
}

std::array<std::array<Bitboard_t, Board::TOTAL_SQUARES>, 2> Precomputations::pawn_attacks;
void Precomputations::calculate_pawn_attacks() {
    for (Square_t square = 0; square < Board::TOTAL_SQUARES; square++) {
        for (Color_t color: {Piece::WHITE, Piece::BLACK}) {
            Bitboard_t moves = 0;
            for (Direction_t direction: Directions::pawn_attack_directions[color]) {
                Square_t target_square = square + direction;
                if (Board::is_valid_square(target_square) && get_squares_to_edge(square, direction) > 0) {
                    Bitboard::set_square(moves, target_square);
                }
            }
            pawn_attacks[color][square] = moves;
        }
    }
}

std::array<Bitboard_t, Board::TOTAL_SQUARES> Precomputations::knight_moves;
void Precomputations::calculate_knight_moves() {
    for (Square_t square = 0; square < Board::TOTAL_SQUARES; square++) {
        Bitboard_t moves = 0;
        for (Direction_t direction: Directions::knight_directions) {
            Square_t target_square = square + direction;
            if (Board::is_valid_square(target_square)) {
                Bitboard::set_square(moves, square + direction);
            }
        }

        for (Direction_t direction: {Directions::EAST, Directions::WEST}) {
            if (get_squares_to_edge(square, direction) < 2) {
                Bitboard::clear_square(moves, square + direction * 2 + Directions::NORTH);
                Bitboard::clear_square(moves, square + direction * 2 + Directions::SOUTH);
                if (get_squares_to_edge(square, direction) == 0) {
                    Bitboard::clear_square(moves, square + direction + Directions::NORTH * 2);
                    Bitboard::clear_square(moves, square + direction + Directions::SOUTH * 2);
                }
            }
        }
        knight_moves[square] = moves;
    }
}

std::array<Bitboard_t, Board::TOTAL_SQUARES> Precomputations::king_moves;
void Precomputations::calculate_king_moves() {
    for (Square_t square = 0; square < Board::TOTAL_SQUARES; square++) {
        Bitboard_t moves = 0;
        for (Direction_t direction: Directions::sliding_directions) {
            Square_t target_square = square + direction;
            if (Board::is_valid_square(target_square) && get_squares_to_edge(square, direction) > 0) {
                Bitboard::set_square(moves, target_square);
            }
        }
        king_moves[square] = moves;
    }
}