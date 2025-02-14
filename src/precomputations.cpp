#include "precomputations.hpp"
#include <iostream>
#include "board.hpp"
#include "directions.hpp"
#include "bitboard.hpp"
#include "piece.hpp"

void Precomputations::init() {
    calculate_squares_to_edge();
    calculate_pawn_attacks();
    calculate_knight_moves();
    calculate_king_moves();
}

std::array<std::array<int, 8>, Board::TOTAL_SQUARES> Precomputations::squares_to_edge;
void Precomputations::calculate_squares_to_edge() {
    for (int rank = 0; rank < Board::RANKS; rank++) {
        for (int file = 0; file < Board::FILES; file++) {
            int square = rank * Board::FILES + file;

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

int Precomputations::get_squares_to_edge(int square, int direction) {
    return squares_to_edge[square][Directions::get_direction_index(direction)];
}

std::array<std::array<uint64_t, Board::TOTAL_SQUARES>, 2> Precomputations::pawn_attacks;
void Precomputations::calculate_pawn_attacks() {
    for (int square = 0; square < Board::TOTAL_SQUARES; square++) {
        for (int color: {Piece::WHITE, Piece::BLACK}) {
            uint64_t moves = 0;
            for (int direction: Directions::pawn_attack_directions[color]) {
                int target_square = square + direction;
                if (Board::is_valid_square(target_square) && get_squares_to_edge(square, direction) > 0) {
                    Bitboard::set_square(moves, target_square);
                }
            }
            pawn_attacks[color][square] = moves;
        }
    }
}

std::array<uint64_t, Board::TOTAL_SQUARES> Precomputations::knight_moves;
void Precomputations::calculate_knight_moves() {
    for (int square = 0; square < Board::TOTAL_SQUARES; square++) {
        uint64_t moves = 0;
        for (int direction: Directions::knight_directions) {
            int target_square = square + direction;
            if (Board::is_valid_square(target_square)) {
                Bitboard::set_square(moves, square + direction);
            }
        }

        for (int direction: {Directions::EAST, Directions::WEST}) {
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

std::array<uint64_t, Board::TOTAL_SQUARES> Precomputations::king_moves;
void Precomputations::calculate_king_moves() {
    for (int square = 0; square < Board::TOTAL_SQUARES; square++) {
        uint64_t moves = 0;
        for (int direction: Directions::sliding_directions) {
            int target_square = square + direction;
            if (Board::is_valid_square(target_square) && get_squares_to_edge(square, direction) > 0) {
                Bitboard::set_square(moves, target_square);
            }
        }
        king_moves[square] = moves;
    }
}