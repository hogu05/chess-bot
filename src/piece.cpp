#include "piece.hpp"

#include <cctype>

#include "board.hpp"
#include "directions.hpp"

PieceType_t Piece::get_piece_type(Piece_t piece)
{
    return (piece & PIECE_TYPE_MASK) >> PIECE_TYPE_SHIFT;
}

Color_t Piece::get_piece_color(Piece_t piece)
{
    return (piece & PIECE_COLOR_MASK) >> PIECE_COLOR_SHIFT;
}

char Piece::get_piece_symbol(Piece_t piece)
{
    char piece_symbol;
    switch (get_piece_type(piece))
    {
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

    piece_symbol = get_piece_color(piece) == WHITE ? std::toupper(piece_symbol) : piece_symbol;
    return piece_symbol;
}

Piece_t Piece::get_piece_from_symbol(char symbol)
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

Color_t Piece::get_other_color(Color_t color)
{
    return color == WHITE ? BLACK : WHITE;
}

Piece_t Piece::create_piece(PieceType_t type, Color_t color)
{
    return (color << PIECE_COLOR_SHIFT) | (type << PIECE_TYPE_SHIFT);
}

bool Piece::can_pawn_move_two_spaces(Square_t square, Color_t color)
{
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == 1) || (color == BLACK && rank == Board::RANKS - 2))
    {
        return true;
    }
    return false;
}

bool Piece::can_pawn_promote(Square_t square, Color_t color)
{
    int rank = Board::get_rank(square);
    if ((color == WHITE && rank == Board::RANKS - 2) || (color == BLACK && rank == 1))
    {
        return true;
    }
    return false;
}

bool Piece::can_move_in_direction(PieceType_t piece_type, Direction_t direction)
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

bool Piece::is_sliding_piece(PieceType_t piece_type)
{
    if (piece_type == BISHOP || piece_type == ROOK || piece_type == QUEEN)
    {
        return true;
    }
    return false;
}

int Piece::get_piece_value(PieceType_t piece)
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
