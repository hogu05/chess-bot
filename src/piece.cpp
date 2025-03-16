#include "piece.hpp"
#include "board.hpp"
#include "directions.hpp"

#include <cctype>
#include <iostream>

PieceType_t Piece::get_piece_type(Piece_t piece) {
    return piece & PIECE_TYPE_MASK;
}

Color_t Piece::get_piece_color(Piece_t piece) {
    return piece & PIECE_COLOR_MASK;
}

char Piece::get_piece_symbol(Piece_t piece) {
    char piece_symbol = ' ';
    switch (get_piece_type(piece)) {
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

    if (get_piece_color(piece) == WHITE) {
        piece_symbol = toupper(piece_symbol);
    }
    return piece_symbol;
}

Piece_t Piece::get_piece_from_symbol(char symbol) {
    Piece_t piece = 0;
    switch (tolower(symbol)) {
        case 'p':
            piece = PAWN;
            break;
        case 'n':
            piece = KNIGHT;
            break;
        case 'b':
            piece = BISHOP;
            break;
        case 'r':
            piece = ROOK;
            break;
        case 'q':
            piece = QUEEN;
            break;
        case 'k':
            piece = KING;
            break;
        default:
            piece = NONE;
    }

    if (islower(symbol)) {
        piece = piece | BLACK;
    }
    return piece;
}

Color_t Piece::get_other_color(Color_t color) {
    return color == WHITE ? BLACK : WHITE;
}


Piece_t Piece::create_piece(PieceType_t piece_type, Color_t color) {
    return piece_type | color;
}

bool Piece::can_pawn_move_two_spaces(Square_t square, Color_t color) {
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == 1) || (color == BLACK && rank == 6)) {
        return true;
    }
    return false;
}

bool Piece::can_pawn_promote(Square_t square, Color_t color) {
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == 6) || (color == BLACK && rank == 1)) {
        return true;
    }
    return false;
}

bool Piece::can_move_in_direction(PieceType_t piece_type, Direction_t direction) {
    if ((piece_type == BISHOP || piece_type == QUEEN) && Directions::is_diagonal_direction(direction)) {
        return true;
    }
    if ((piece_type == ROOK || piece_type == QUEEN) && Directions::is_orthogonal_direction(direction)) {
        return true;
    }
    return false;
}

bool Piece::is_sliding_piece(PieceType_t piece_type) {
    if (piece_type == BISHOP || piece_type == ROOK || piece_type == QUEEN) {
        return true;
    }
    return false;
}
