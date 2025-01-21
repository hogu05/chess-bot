#include "move_generator.h"

#include <iostream>
#include <ostream>

#include "bitboard.h"
#include "piece.h"
#include "precomputations.h"
#include "move.h"

MoveGenerator::MoveGenerator(const Board board) : board(board) {

}

std::vector<int> MoveGenerator::generate_moves() {
    std::vector<int> moves;
    for (int square = 0; square < Board::TOTAL_SQUARES; square++) {
        if (board.is_occupied(square) && board.get_piece_color(square) == board.to_move) {
            std::vector<int> piece_moves = generate_piece_moves(board.pieces[square], square, board.to_move);
            moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
        }
    }
    return moves;
}

std::vector<int> MoveGenerator::generate_piece_moves(int piece, int square, int color) {
    switch (Piece::get_piece_type(piece)) {
        case Piece::PAWN: return generate_pawn_moves(square, color);
        case Piece::KNIGHT: return generate_knight_moves(square, color);
        case Piece::BISHOP:
        case Piece::ROOK:
        case Piece::QUEEN: return generate_sliding_piece_moves(square, Piece::get_piece_type(piece), color);
        case Piece::KING: return generate_king_moves(square, color);
        default: return {};
    }
}

std::vector<int> MoveGenerator::generate_pawn_moves(int square, int color) {
    std::vector<int> moves;
    int direction = Directions::pawn_directions[color];

    if (board.is_empty(square + direction)) {
        moves.push_back(Move::get_move(square, square + direction));
        if (Piece::can_pawn_move_two_spaces(square, color) && board.is_empty(square + direction * 2)) {
            moves.push_back(Move::get_move(square, square + direction * 2));
        }
    }

    uint64_t pawn_attacks_bitboard = Precomputations::pawn_attacks[color][square];
    while (pawn_attacks_bitboard != 0) {
        int target_square = Bitboard::pop_square(pawn_attacks_bitboard);
        if (board.is_occupied(target_square) && board.get_piece_color(target_square) != color) {
            moves.push_back(Move::get_move(target_square, target_square));
        }

        if (target_square == board.en_passant) {
            moves.push_back(Move::get_move(target_square, target_square));
        }

    }

    return moves;
}

std::vector<int> MoveGenerator::generate_knight_moves(int square, int color) {
    std::vector<int> moves;

    uint64_t knight_moves_bitboard = Precomputations::knight_moves[square];
    while (knight_moves_bitboard != 0) {
        int target_square = Bitboard::pop_square(knight_moves_bitboard);
        if (board.is_empty(target_square) || board.get_piece_color(target_square) != color) {
            moves.push_back(Move::get_move(square, target_square));
        }
    }

    return moves;
}

std::vector<int> MoveGenerator::generate_sliding_piece_moves(int square, int piece, int color) {
    std::vector<int> moves;

    int start_index = 0;
    int end_index = 7;
    if (piece == Piece::BISHOP) start_index += 4;
    if (piece == Piece::ROOK) end_index -= 4;

    for (int direction_index = start_index; direction_index <= end_index; direction_index++) {
        int direction = Directions::sliding_directions[direction_index];
        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++) {
            int target_square = square + direction * i;
            if (board.is_occupied(target_square)) {
                if (board.get_piece_color(target_square) != color) {
                    moves.push_back(Move::get_move(square, target_square));
                }
                break;
            }
            moves.push_back(Move::get_move(square, target_square));
        }
    }
    return moves;
}

std::vector<int> MoveGenerator::generate_king_moves(int square, int color) {
    std::vector<int> moves;

    uint64_t king_moves_bitboard = Precomputations::king_moves[square];
    while (king_moves_bitboard != 0) {
        int target_square = Bitboard::pop_square(king_moves_bitboard);
        if (board.is_empty(target_square) || board.get_piece_color(target_square) != color) {
            moves.push_back(Move::get_move(square, target_square));
        }
    }

    if (board.short_castle[color] && board.is_empty(square + 1) && board.is_empty(square + 2)) {
        moves.push_back(Move::get_move(square, square + 2));
    }

    if (board.long_castle[color] && board.is_empty(square - 1) && board.is_empty(square - 2) && board.is_empty(square - 3)) {
        moves.push_back(Move::get_move(square, square - 3));
    }

    return moves;
}

