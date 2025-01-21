#include <iostream>
#include "board.h"

#include "piece.h"
#include <string>

void Board::load_position_from_fen(std::string fen) {
    int square = 56;
    for (char current_char: fen) {
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

int Board::get_file(int square) {
    return square % RANKS;
}

int Board::get_rank(int square) {
    return square / FILES;
}

int Board::get_piece_color(int square) {
    return Piece::get_piece_color(pieces[square]);
}

bool Board::is_valid_square(int square) {
    return square >= 0 && square < TOTAL_SQUARES;
}
