#include "move_generator.h"

#include <iostream>
#include <ostream>

#include "bitboard.h"
#include "piece.h"
#include "precomputations.h"
#include "move.h"

MoveGenerator::MoveGenerator(const Board board) : board(board){}

void MoveGenerator::generate_bitboards() {
    Bitboard::clear_all(friendly_pieces_bitboard);
    Bitboard::clear_all(enemy_pieces_bitboard);
    Bitboard::clear_all(all_pieces_bitboard);
    Bitboard::clear_all(friendly_king_bitboard);
    for (int square = 0; square < Board::TOTAL_SQUARES; square++) {
        if (board.is_occupied(square)) {
            Bitboard::set_square(all_pieces_bitboard, square);
            if (board.get_piece_color(square) == board.position_info.to_move) {
                Bitboard::set_square(friendly_pieces_bitboard, square);
                if (board.get_piece_type(square) == Piece::KING) {
                    Bitboard::set_square(friendly_king_bitboard, square);
                }
            } else {
                Bitboard::set_square(enemy_pieces_bitboard, square);
            }
        }
    }

    Bitboard::clear_all(attacked_squares_bitboard);
    Bitboard::clear_all(checking_piece_bitboard);
    is_double_check = false;
    generate_attacked_squares_bitboard();

    Bitboard::clear_all(pinned_pieces_bitboard);
    Bitboard::set_all(blocking_squares_bitboard);
    if (!is_double_check) {
        generate_pinned_pieces_bitboard();
        if (checking_piece_bitboard != 0) {
            generate_blocking_squares_bitboard();
        }
    }
}


std::vector<int> MoveGenerator::generate_moves() {
    generate_bitboards();
    std::vector<int> moves;
    if (!is_double_check) {
        uint64_t moving_pieces_bitboard = friendly_pieces_bitboard;
        while (moving_pieces_bitboard != 0) {
            int square = Bitboard::pop_square(moving_pieces_bitboard);
            Bitboard::set_all(pinned_piece_possible_squares_bitboard);
            if (Bitboard::is_set(pinned_pieces_bitboard, square)) {
                Bitboard::clear_all(pinned_piece_possible_squares_bitboard);
                generate_pinned_piece_possible_squares_bitboard(square);
            }
            std::vector<int> piece_moves = generate_piece_moves(square);
            moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
        }
    } else {
        moves = generate_piece_moves(Bitboard::get_square(friendly_pieces_bitboard));
    }
    return moves;
}

std::vector<int> MoveGenerator::generate_piece_moves(int square) {
    int piece_type = board.get_piece_type(square);
    int color = board.get_piece_color(square);
    switch (piece_type) {
        case Piece::PAWN:
            return generate_pawn_moves(square, color);
        case Piece::KNIGHT:
            return generate_knight_moves(square);
        case Piece::BISHOP:
        case Piece::ROOK:
        case Piece::QUEEN:
        return generate_sliding_piece_moves(square, piece_type);
        case Piece::KING:
            return generate_king_moves(square, color);
        default:
            return {};
    }
}

std::vector<int> MoveGenerator::generate_pawn_moves(int square, int color) {
    std::vector<int> moves;

    int direction = Directions::pawn_directions[color];

    if (Bitboard::is_clear(all_pieces_bitboard, square + direction)) {
        if (get_legal_moves(Bitboard::create_bitboard(square + direction)) != 0) {
            moves.push_back(Move::create_move(square, square + direction, Move::NO_FLAG));
        }
        if (Piece::can_pawn_move_two_spaces(square, color) && Bitboard::is_clear(all_pieces_bitboard,square + direction * 2)) {
            if (get_legal_moves(Bitboard::create_bitboard(square + direction * 2)) != 0) {
                moves.push_back(Move::create_move(square, square + direction * 2, Move::TWO_SPACE_PAWN_MOVE_FLAG));
            }
        }
    }

    uint64_t pseudo_legal_moves_bitboard = Precomputations::pawn_attacks[color][square] & enemy_pieces_bitboard;
    uint64_t legal_moves_bitboard = get_legal_moves(pseudo_legal_moves_bitboard);
    std::vector<int> legal_moves = Move::create_moves_from_bitboard(square, legal_moves_bitboard);
    moves.insert(moves.end(), legal_moves.begin(), legal_moves.end());

    if (Bitboard::is_set(Precomputations::pawn_attacks[color][square], board.position_info.en_passant)) {
        // TODO: En passant
        moves.push_back(Move::create_move(square, board.position_info.en_passant, Move::EN_PASSANT_FLAG));
    }

    if (Piece::can_pawn_promote(square, color)) {
        moves = generate_pawn_promotion_moves(moves);
    }

    return moves;
}

std::vector<int> MoveGenerator::generate_pawn_promotion_moves(std::vector<int> moves) {
    std::vector<int> promotion_moves;
    for (int move: moves) {
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_KNIGHT_FLAG));
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_BISHOP_FLAG));
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_ROOK_FLAG));
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_QUEEN_FLAG));
    }
    return promotion_moves;
}

std::vector<int> MoveGenerator::generate_knight_moves(int square) {
    uint64_t pseudo_legal_moves_bitboard = Precomputations::knight_moves[square] & (~friendly_pieces_bitboard);
    uint64_t legal_moves_bitboard = get_legal_moves(pseudo_legal_moves_bitboard);
    return Move::create_moves_from_bitboard(square, legal_moves_bitboard);
}

std::vector<int> MoveGenerator::generate_sliding_piece_moves(int square, int piece) {
    uint64_t pseudo_legal_moves_bitboard = 0;
    int start_index = 0;
    int end_index = 7;
    if (piece == Piece::BISHOP) start_index += 4;
    if (piece == Piece::ROOK) end_index -= 4;

    for (int direction_index = start_index; direction_index <= end_index; direction_index++) {
        int direction = Directions::sliding_directions[direction_index];
        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++) {
            int target_square = square + direction * i;
            if (Bitboard::is_set(all_pieces_bitboard, target_square)) {
                if (Bitboard::is_set(enemy_pieces_bitboard, target_square)) {
                    Bitboard::set_square(pseudo_legal_moves_bitboard, target_square);
                }
                break;
            }
            Bitboard::set_square(pseudo_legal_moves_bitboard, target_square);;
        }
    }
    uint64_t legal_moves_bitboard = get_legal_moves(pseudo_legal_moves_bitboard);
    return Move::create_moves_from_bitboard(square, legal_moves_bitboard);
}

std::vector<int> MoveGenerator::generate_king_moves(int square, int color) {
    uint64_t pseudo_legal_moves_bitboard = Precomputations::king_moves[square] & (~friendly_pieces_bitboard);
    uint64_t legal_moves_bitboard = pseudo_legal_moves_bitboard & (~attacked_squares_bitboard);
    std::vector<int> moves = Move::create_moves_from_bitboard(square, legal_moves_bitboard);

    if (board.position_info.can_short_castle[color] &&
            Bitboard::is_clear(all_pieces_bitboard, square + 1) &&
            Bitboard::is_clear(all_pieces_bitboard, square + 2)) {
        if (Bitboard::is_clear(attacked_squares_bitboard, square + 1) &&
            Bitboard::is_clear(attacked_squares_bitboard, square + 2)) {
            moves.push_back(Move::create_move(square, square + 2, Move::CASTLE_FLAG));
        }
    }

    if (board.position_info.can_short_castle[color] &&
            Bitboard::is_clear(all_pieces_bitboard, square - 1) &&
            Bitboard::is_clear(all_pieces_bitboard, square - 2) &&
            Bitboard::is_clear(all_pieces_bitboard, square - 3)) {
        if (Bitboard::is_clear(attacked_squares_bitboard, square - 1)
            && Bitboard::is_clear(attacked_squares_bitboard, square - 2)) {
            moves.push_back(Move::create_move(square, square - 2, Move::CASTLE_FLAG));
        }
    }

    return moves;
}

void MoveGenerator::generate_attacked_squares_bitboard() {
    uint64_t attacking_pieces_bitboard = enemy_pieces_bitboard;
    while (attacking_pieces_bitboard != 0) {
        int square = Bitboard::pop_square(attacking_pieces_bitboard);
        uint64_t piece_attacks = generate_piece_attacks(square);
        attacked_squares_bitboard = attacked_squares_bitboard | piece_attacks;
    }
}

uint64_t MoveGenerator::generate_piece_attacks(int square) {
    uint64_t attacks = 0;
    int piece_type = board.get_piece_type(square);
    int color = board.get_piece_color(square);
    switch (piece_type) {
        case Piece::PAWN:
            attacks = generate_pawn_attacks(square, color);
            break;
        case Piece::KNIGHT:
            attacks = generate_knight_attacks(square);
            break;
        case Piece::BISHOP:
        case Piece::ROOK:
        case Piece::QUEEN:
            attacks = generate_sliding_piece_attacks(square, piece_type);
            break;
        case Piece::KING:
            attacks = generate_king_attacks(square);
            break;
        default:
            attacks = 0;
    }
    if ((attacks & friendly_king_bitboard) != 0) {
        if (checking_piece_bitboard == 0) {
            Bitboard::set_square(checking_piece_bitboard, square);
        } else {
            is_double_check = true;
        }
    }
    return attacks;
}

uint64_t MoveGenerator::generate_pawn_attacks(int square, int color) {
    return Precomputations::pawn_attacks[color][square];
}

uint64_t MoveGenerator::generate_knight_attacks(int square) {
    return Precomputations::knight_moves[square];
}

uint64_t MoveGenerator::generate_sliding_piece_attacks(int square, int piece) {
    uint64_t attacks = 0;

    int start_index = 0;
    int end_index = 7;
    if (piece == Piece::BISHOP) start_index += 4;
    if (piece == Piece::ROOK) end_index -= 4;

    for (int direction_index = start_index; direction_index <= end_index; direction_index++) {
        int direction = Directions::sliding_directions[direction_index];
        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++) {
            int target_square = square + direction * i;
            if (Bitboard::is_set(all_pieces_bitboard, target_square) && Bitboard::is_clear(friendly_king_bitboard, target_square)) {
                break;
            }
            Bitboard::set_square(attacks, target_square);
        }
    }
    return attacks;
}

uint64_t MoveGenerator::generate_king_attacks(int square) {
    return Precomputations::king_moves[square];
}

void MoveGenerator::generate_pinned_pieces_bitboard() {
    int square = Bitboard::get_square(friendly_king_bitboard);
    for (int direction: Directions::sliding_directions) {
        int pinned_piece_square = -1;

        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++) {
            int target_square = square + direction * i;
            if (Bitboard::is_set(friendly_pieces_bitboard, target_square)) {
                if (pinned_piece_square == -1) {
                    pinned_piece_square = target_square;
                } else {
                    break;
                }
            }
            if (Bitboard::is_set(enemy_pieces_bitboard, target_square)) {
                if (pinned_piece_square != -1) {
                    if (Piece::can_move_in_direction(board.get_piece_type(target_square), -1 * direction)) {
                        Bitboard::set_square(pinned_pieces_bitboard, pinned_piece_square);
                    }
                }
                break;
            }
        }
    }
}

void MoveGenerator::generate_blocking_squares_bitboard() {
    int king_square = Bitboard::get_square(friendly_king_bitboard);
    int checking_piece_square = Bitboard::get_square(checking_piece_bitboard);
    if (!Piece::is_sliding_piece(board.get_piece_type(checking_piece_square))) {
        return;
    }
    Bitboard::clear_all(blocking_squares_bitboard);
    int direction = Directions::get_ray_direction(king_square, checking_piece_square);
    int target_square = king_square + direction;
    while (target_square != checking_piece_square) {
        Bitboard::set_square(blocking_squares_bitboard, target_square);
        target_square += direction;
    }
}

void MoveGenerator::generate_pinned_piece_possible_squares_bitboard(int square) {
    int king_square = Bitboard::get_square(friendly_king_bitboard);
    int direction = Directions::get_ray_direction(king_square, square);
    int target_square = king_square + direction;
    while (Bitboard::is_clear(enemy_pieces_bitboard, target_square)) {
        Bitboard::set_square(pinned_piece_possible_squares_bitboard, target_square);
        target_square += direction;
    }
}

uint64_t MoveGenerator::get_legal_moves(uint64_t moves) {
    return moves & (blocking_squares_bitboard | checking_piece_bitboard) & pinned_piece_possible_squares_bitboard;
}




