#include "piece.h"
#include <cctype>
#include <iostream>

#include "board.h"
#include "directions.h"

int Piece::get_piece_type(int piece_index) {
    return piece_index & PIECE_TYPE_MASK;
}

int Piece::get_piece_color(int piece_index) {
    return piece_index & PIECE_COLOR_MASK;
}

char Piece::get_piece_symbol(int piece_index) {
    char piece_symbol = ' ';
    switch (get_piece_type(piece_index)) {
        case PAWN:
            piece_symbol = 'p';
            break;
        case KNIGHT:
            piece_symbol = 'n';
            break;
        case BISHOP:
            piece_symbol = 'b';
        break;
        case ROOK:
            piece_symbol = 'r';
            break;
        case QUEEN:
            piece_symbol = 'q';
            break;
        case KING:
            piece_symbol = 'k';
            break;
        default:
            piece_symbol = '.';
    }

    if (get_piece_color(piece_index) == WHITE) {
        piece_symbol = toupper(piece_symbol);
    }
    return piece_symbol;
}

int Piece::get_piece_from_symbol(char symbol) {
    int piece_index = 0;
    switch (tolower(symbol)) {
        case 'p':
            piece_index = PAWN;
            break;
        case 'n':
            piece_index = KNIGHT;
            break;
        case 'b':
            piece_index = BISHOP;
            break;
        case 'r':
            piece_index = ROOK;
            break;
        case 'q':
            piece_index = QUEEN;
            break;
        case 'k':
            piece_index = KING;
            break;
        default:
            piece_index = NONE;
    }

    if (islower(symbol)) {
        piece_index = piece_index | BLACK;
    }
    return piece_index;
}

int Piece::get_other_color(int color) {
    return color == WHITE ? BLACK : WHITE;
}


int Piece::create_piece(int piece_type, int color) {
    return piece_type | color;
}

bool Piece::can_pawn_move_two_spaces(int square, int color) {
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == 1) || (color == BLACK && rank == 6)) {
        return true;
    }
    return false;
}

bool Piece::can_pawn_promote(int square, int color) {
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == 6) || (color == BLACK && rank == 1)) {
        return true;
    }
    return false;
}

bool Piece::can_move_in_direction(int piece_type, int direction) {
    if ((piece_type == BISHOP || piece_type == QUEEN) && Directions::is_diagonal_direction(direction)) {
        return true;
    }
    if ((piece_type == ROOK || piece_type == QUEEN) && Directions::is_orthogonal_direction(direction)) {
        return true;
    }
    return false;
}

bool Piece::is_sliding_piece(int piece_type) {
    if (piece_type == BISHOP || piece_type == ROOK || piece_type == QUEEN) {
        return true;
    }
    return false;
}
