#include <iostream>
#include "board.h"

#include "piece.h"
#include <string>
#include <sstream>
#include "move.h"

int Board::get_file(int square) {
    return square % RANKS;
}

int Board::get_rank(int square) {
    return square / FILES;
}

int Board::get_square(int file, int rank) {
    return rank * FILES + file;
}

bool Board::is_valid_square(int square) {
    return square >= 0 && square < TOTAL_SQUARES;
}

int Board::get_file_from_notation(char notation) {
    switch (notation) {
        case 'a':
            return 0;
        case 'b':
            return 1;
        case 'c':
            return 2;
        case 'd':
            return 3;
        case 'e':
            return 4;
        case 'f':
            return 5;
        case 'g':
            return 6;
        case 'h':
            return 7;
        default:
            return 0;
    }
}

int Board::get_square_from_notation(std::string notation) {
    int file = get_file_from_notation(notation[0]);
    int rank = notation[1] - '0' - 1;
    return get_square(file, rank);
}


void Board::load_position_from_fen(std::string fen) {
    std::istringstream stream(fen);
    std::array<std::string, 6> fen_info;
    stream >> fen_info[0] >> fen_info[1] >> fen_info[2] >> fen_info[3] >> fen_info[4] >> fen_info[5];

    // Getting pieces
    int square = 56;
    for (char current_char: fen_info[0]) {
        if (current_char == '/') {
            square -= 16;
            continue;
        }
        if (std::isdigit(current_char)) {
            square += current_char - '0';
        } else {
            pieces[square] = Piece::get_piece_from_symbol(current_char);
            square++;
        }
    }

    // Getting to_move color
    if (fen_info[1] == "w") {
        position_info.to_move = Piece::WHITE;
    }

    if (fen_info[1] == "b") {
        position_info.to_move = Piece::BLACK;
    }

    // Getting castling rights
    for (char current_char: fen_info[2]) {
        switch (current_char) {
            case 'K':
                position_info.can_short_castle[Piece::WHITE] = true;
                break;
            case 'Q':
                position_info.can_long_castle[Piece::WHITE] = true;
                break;
            case 'k':
                position_info.can_short_castle[Piece::BLACK] = true;
                break;
            case 'q':
                position_info.can_long_castle[Piece::BLACK] = true;
                break;
            default:
                break;
        }
    }

    // Getting en passant
    if (!fen_info[3].empty() && fen_info[3] != "-") {
        position_info.en_passant = get_square_from_notation(fen_info[3]);
    }

    // Getting fifty move rule half-moves
    if (!fen_info[4].empty()) {
        position_info.fifty_move_ply = std::stoi(fen_info[4]);
    }

    // Getting full moves
    if (!fen_info[5].empty()) {
        // Full moves of the game
    }

    previous_positions.push(position_info);
}

void Board::print_board() {
    int square = 56;
    while (square >= 0) {
        std::cout << Piece::get_piece_symbol(pieces[square]) << " ";
        square++;
        if (square % 8 == 0) {
            std::cout << std::endl;
            square -= 16;
        }
    }
    std::cout << std::endl;
}

bool Board::is_occupied(int square) {
    return Piece::get_piece_type(pieces[square]) != Piece::NONE;
}

bool Board::is_empty(int square) {
    return Piece::get_piece_type(pieces[square]) == Piece::NONE;
}

int Board::get_piece_color(int square) {
    return Piece::get_piece_color(pieces[square]);
}


void Board::move_piece(int start_square, int target_square) {
    pieces[target_square] = pieces[start_square];
    pieces[start_square] = Piece::NONE;
}

void Board::make_move(int move) {
    PositionInfo new_position_info;
    new_position_info.can_short_castle = position_info.can_short_castle;
    new_position_info.can_long_castle = position_info.can_long_castle;
    int start_square = Move::get_start_square(move);
    int target_square = Move::get_target_square(move);
    int move_flag = Move::get_flag(move);
    int moving_color = position_info.to_move;
    int next_move_color = moving_color == Piece::WHITE? Piece::BLACK : Piece::WHITE;

    new_position_info.to_move = next_move_color;

    bool is_capture = is_occupied(target_square);
    if (is_capture) {
        new_position_info.captured_piece = pieces[target_square];
    }

    if (move_flag == Move::TWO_SPACE_PAWN_MOVE_FLAG) {
        new_position_info.en_passant = (start_square + target_square) / 2;
    }

    if (move_flag == Move::CASTLE_FLAG) {
        if (target_square == start_square + 2) { // Short castle
            move_piece(start_square + 3, start_square + 1); // Moves rook
        }
        if (target_square == start_square - 2) { // Long castle
            move_piece(start_square - 4, start_square - 1); // Moves rook
        }
        new_position_info.can_short_castle[moving_color] = false;
        new_position_info.can_long_castle[moving_color] = false;
    }

    move_piece(start_square, target_square);

    if (Move::is_pawn_promotion(move)) {
        int promoted_piece_type = Move::get_pawn_promotion_piece_type(move);
        pieces[target_square] = Piece::create_piece(promoted_piece_type, moving_color);
    }

    if (position_info.can_short_castle[moving_color]) {
        if (start_square == KINGSIDE_ROOK_START_SQUARE[moving_color] ||
            start_square == KING_START_SQUARE[moving_color]) {
            new_position_info.can_short_castle[moving_color] = false;
        }
    }

    if (position_info.can_long_castle[moving_color]) {
        if (start_square == QUEENSIDE_ROOK_START_SQUARE[moving_color] ||
            start_square == KING_START_SQUARE[moving_color]) {
            new_position_info.can_long_castle[moving_color] = false;
        }
    }

    if (position_info.can_short_castle[next_move_color]) {
        if (target_square == KINGSIDE_ROOK_START_SQUARE[next_move_color]) {
            new_position_info.can_short_castle[next_move_color] = false;
        }
    }

    if (position_info.can_long_castle[next_move_color]) {
        if (target_square == QUEENSIDE_ROOK_START_SQUARE[next_move_color]) {
            new_position_info.can_long_castle[next_move_color] = false;
        }
    }

    // TODO: fifty move rule
    // TODO: threefold

    previous_positions.push(position_info);
    position_info = new_position_info;
}

void Board::unmake_move(int move) {
    int start_square = Move::get_start_square(move);
    int target_square = Move::get_target_square(move);
    int move_flag = Move::get_flag(move);

    PositionInfo previous_position_info = previous_positions.top();
    previous_positions.pop();


    if (Move::is_pawn_promotion(move)) {
        pieces[target_square] = Piece::create_piece(Piece::PAWN, previous_position_info.to_move);
    }

    move_piece(target_square, start_square);

    bool is_capture = position_info.captured_piece != Piece::NONE;
    if (is_capture) {
        pieces[target_square] = position_info.captured_piece;
    }


    if (move_flag == Move::CASTLE_FLAG) {
        if (target_square == start_square + 2) { // Short castle
            move_piece(start_square + 1, start_square + 3); // Moves rook
        }
        if (target_square == start_square - 2) { // Long castle
            move_piece(start_square - 1, start_square - 4); // Moves rook
        }
    }

    // TODO: fifty move rule
    // TODO: threefold

    position_info = previous_position_info;
}