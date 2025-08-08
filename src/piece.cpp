#include "piece.hpp"

#include <cctype>
#include <string>

#include "board.hpp"
#include "directions.hpp"
#include "types.hpp"

namespace Piece
{
PieceType_t get_piece_type(Piece_t piece)
{
    return (piece & PIECE_TYPE_MASK) >> PIECE_TYPE_SHIFT;
}

Color_t get_piece_color(Piece_t piece)
{
    return (piece & PIECE_COLOR_MASK) >> PIECE_COLOR_SHIFT;
}

std::string get_piece_symbol(Piece_t piece, DisplayMode display_mode)
{
    bool isWhite = (get_piece_color(piece) == WHITE);

    if (display_mode == DisplayMode::UNICODE)
    {
        switch (get_piece_type(piece))
        {
        case PAWN:
            return isWhite ? "♟" : "♙";
        case KNIGHT:
            return isWhite ? "♞" : "♘";
        case BISHOP:
            return isWhite ? "♝" : "♗";
        case ROOK:
            return isWhite ? "♜" : "♖";
        case QUEEN:
            return isWhite ? "♛" : "♕";
        case KING:
            return isWhite ? "♚" : "♔";
        default:
            return ".";
        }
    }
    else
    {
        switch (get_piece_type(piece))
        {
        case PAWN:
            return isWhite ? "P" : "p";
        case KNIGHT:
            return isWhite ? "N" : "n";
        case BISHOP:
            return isWhite ? "B" : "b";
        case ROOK:
            return isWhite ? "R" : "r";
        case QUEEN:
            return isWhite ? "Q" : "q";
        case KING:
            return isWhite ? "K" : "k";
        default:
            return ".";
        }
    }
}

Piece_t get_piece_from_symbol(char symbol)
{
    PieceType_t piece_type;
    switch (tolower(symbol))
    {
    case 'p':
        piece_type = PAWN;
        break;
    case 'n':
        piece_type = KNIGHT;
        break;
    case 'b':
        piece_type = BISHOP;
        break;
    case 'r':
        piece_type = ROOK;
        break;
    case 'q':
        piece_type = QUEEN;
        break;
    case 'k':
        piece_type = KING;
        break;
    default:
        piece_type = NONE;
    }

    Color_t piece_color = std::isupper(symbol) ? WHITE : BLACK;

    return create_piece(piece_type, piece_color);
}

Color_t get_other_color(Color_t color)
{
    return color == WHITE ? BLACK : WHITE;
}

Piece_t create_piece(PieceType_t type, Color_t color)
{
    return (color << PIECE_COLOR_SHIFT) | (type << PIECE_TYPE_SHIFT);
}

bool can_pawn_move_two_spaces(Square_t square, Color_t color)
{
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == 1) || (color == BLACK && rank == Board::RANKS - 2))
    {
        return true;
    }
    return false;
}

bool can_pawn_promote(Square_t square, Color_t color)
{
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == Board::RANKS - 2) || (color == BLACK && rank == 1))
    {
        return true;
    }
    return false;
}

bool can_move_in_direction(PieceType_t piece_type, Direction_t direction)
{
    if ((piece_type == BISHOP || piece_type == QUEEN) &&
        Directions::is_diagonal_direction(direction))
    {
        return true;
    }
    if ((piece_type == ROOK || piece_type == QUEEN) &&
        Directions::is_orthogonal_direction(direction))
    {
        return true;
    }
    return false;
}

bool is_sliding_piece(PieceType_t piece_type)
{
    if (piece_type == BISHOP || piece_type == ROOK || piece_type == QUEEN)
    {
        return true;
    }
    return false;
}

int get_piece_value(PieceType_t piece)
{
    switch (piece)
    {
    case PAWN:
        return 100;
    case KNIGHT:
    case BISHOP:
        return 300;
    case ROOK:
        return 500;
    case QUEEN:
        return 900;
    case KING:
    default:
        return 0;
    }
}

int get_piece_index(Piece_t piece)
{
    PieceType_t piece_type = get_piece_type(piece);

    if (piece_type == NONE)
    {
        return -1;
    }

    int index = piece_type - 1;
    if (get_piece_color(piece) == BLACK)
    {
        index += PIECE_TYPE_COUNT;
    }
    return index;
}
} // namespace Piece
