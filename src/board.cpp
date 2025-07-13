#include "board.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "move.hpp"
#include "piece.hpp"
#include "position_info.hpp"

int Board::get_file(int square)
{
    return square & 0b111;
}

int Board::get_rank(int square)
{
    return square >> 3;
}

Square_t Board::get_square(int file, int rank)
{
    return (rank << 3) | file;
}

bool Board::is_valid_square(Square_t square)
{
    return square >= 0 && square < TOTAL_SQUARES;
}

int Board::get_file_from_notation(char notation)
{
    return notation - 'a';
}

char Board::get_file_notation(int file)
{
    return 'a' + file;
}

Square_t Board::get_square_from_notation(std::string notation)
{
    int file = get_file_from_notation(notation[0]);
    int rank = notation[1] - '0' - 1;
    return get_square(file, rank);
}

std::string Board::get_square_notation(int square)
{
    std::string notation;
    char file = get_file_notation(get_file(square));
    char rank = get_rank(square) + 1 + '0';
    notation += file;
    notation += rank;
    return notation;
}

Square_t Board::get_en_passant_square(int en_passant_file, Color_t to_move)
{
    if (en_passant_file == -1)
    {
        return -1;
    }
    int rank = to_move == Piece::WHITE ? 5 : 2;
    return get_square(en_passant_file, rank);
}

Square_t Board::get_en_passant_capture_square(int start_square, Color_t target_square)
{
    int file = get_file(target_square);
    int rank = get_rank(start_square);
    return get_square(file, rank);
}

void Board::load_position_from_fen(std::string fen)
{
    reset();

    std::istringstream stream(fen);
    std::array<std::string, 6> fen_info;
    stream >> fen_info[0] >> fen_info[1] >> fen_info[2] >> fen_info[3] >> fen_info[4] >>
        fen_info[5];

    // Getting pieces
    Square_t square = 56;
    for (char current_char : fen_info[0])
    {
        if (current_char == '/')
        {
            square -= 16;
            continue;
        }
        if (std::isdigit(current_char))
        {
            square += current_char - '0';
        }
        else
        {
            pieces[square] = Piece::get_piece_from_symbol(current_char);
            square++;
        }
    }

    // Getting to_move color
    if (fen_info[1] == "w")
    {
        PositionInfo::set_to_move(position_info, Piece::WHITE);
    }

    if (fen_info[1] == "b")
    {
        PositionInfo::set_to_move(position_info, Piece::BLACK);
        std::cout << position_info << std::endl;
    }

    // Getting castling rights
    for (char current_char : fen_info[2])
    {
        switch (current_char)
        {
        case 'K':
            PositionInfo::set_castling_right(position_info, Piece::WHITE, true, true);
            break;
        case 'Q':
            PositionInfo::set_castling_right(position_info, Piece::WHITE, false, true);
            break;
        case 'k':
            PositionInfo::set_castling_right(position_info, Piece::BLACK, true, true);
            break;
        case 'q':
            PositionInfo::set_castling_right(position_info, Piece::BLACK, false, true);
            break;
        default:
            break;
        }
    }

    // Getting en passant
    if (!fen_info[3].empty() && fen_info[3] != "-")
    {
        PositionInfo::set_en_passant(position_info, true,
                                     get_file(get_square_from_notation(fen_info[3])));
    }

    // Getting fifty move rule half-moves
    if (!fen_info[4].empty())
    {
        PositionInfo::set_fifty_move_ply(position_info, std::stoi(fen_info[4]));
    }

    // Getting full moves
    if (!fen_info[5].empty())
    {
        // Full moves of the game
    }

    previous_positions.push(position_info);
}

void Board::print_board()
{
    Square_t square = 56;
    while (square >= 0)
    {
        std::cout << Piece::get_piece_symbol(pieces[square]) << " ";
        square++;
        if (square % 8 == 0)
        {
            std::cout << std::endl;
            square -= 16;
        }
    }
    std::cout << std::endl;
}

bool Board::is_occupied(Square_t square)
{
    return Piece::get_piece_type(pieces[square]) != Piece::NONE;
}

bool Board::is_empty(Square_t square)
{
    return Piece::get_piece_type(pieces[square]) == Piece::NONE;
}

Color_t Board::get_piece_color(Square_t square)
{
    return Piece::get_piece_color(pieces[square]);
}

PieceType_t Board::get_piece_type(Square_t square)
{
    return Piece::get_piece_type(pieces[square]);
}

void Board::move_piece(Square_t start_square, Square_t target_square)
{
    pieces[target_square] = pieces[start_square];
    pieces[start_square] = Piece::NONE;
}

void Board::make_move(Move_t move)
{
    PositionInfo_t new_position_info = 0;
    PositionInfo::set_castling_rights(new_position_info,
                                      PositionInfo::get_castling_rights(position_info));
    Square_t start_square = Move::get_start_square(move);
    Square_t target_square = Move::get_target_square(move);
    int move_flag = Move::get_flag(move);
    Color_t moving_color = PositionInfo::get_to_move(position_info);
    Color_t next_move_color = Piece::get_other_color(moving_color);

    PositionInfo::set_to_move(new_position_info, next_move_color);

    bool is_capture = is_occupied(target_square);
    if (is_capture)
    {
        PositionInfo::set_captured_piece(new_position_info, pieces[target_square]);
    }

    if (move_flag == Move::EN_PASSANT_FLAG)
    {
        Square_t en_passant_capture_square =
            get_en_passant_capture_square(start_square, target_square);
        PositionInfo::set_captured_piece(new_position_info, pieces[en_passant_capture_square]);
        pieces[en_passant_capture_square] = Piece::NONE;
    }

    if (move_flag == Move::TWO_SPACE_PAWN_MOVE_FLAG)
    {
        PositionInfo::set_en_passant(new_position_info, true, get_file(start_square));
    }

    if (move_flag == Move::CASTLE_FLAG)
    {
        if (target_square == start_square + 2)
        {
            // Short castle
            move_piece(start_square + 3, start_square + 1); // Moves rook
        }
        if (target_square == start_square - 2)
        {
            // Long castle
            move_piece(start_square - 4, start_square - 1); // Moves rook
        }
        PositionInfo::set_castling_right(new_position_info, moving_color, true, false);
        PositionInfo::set_castling_right(new_position_info, moving_color, false, false);
    }

    move_piece(start_square, target_square);

    if (Move::is_pawn_promotion(move))
    {
        PieceType_t promoted_piece_type = Move::get_pawn_promotion_piece_type(move);
        pieces[target_square] = Piece::create_piece(promoted_piece_type, moving_color);
    }

    if (PositionInfo::get_castling_right(position_info, moving_color, true))
    {
        if (start_square == KINGSIDE_ROOK_START_SQUARE[moving_color] ||
            start_square == KING_START_SQUARE[moving_color])
        {
            PositionInfo::set_castling_right(new_position_info, moving_color, true, false);
        }
    }

    if (PositionInfo::get_castling_right(position_info, moving_color, false))
    {
        if (start_square == QUEENSIDE_ROOK_START_SQUARE[moving_color] ||
            start_square == KING_START_SQUARE[moving_color])
        {
            PositionInfo::set_castling_right(new_position_info, moving_color, false, false);
        }
    }

    if (PositionInfo::get_castling_right(position_info, next_move_color, true))
    {
        if (target_square == KINGSIDE_ROOK_START_SQUARE[next_move_color])
        {
            PositionInfo::set_castling_right(new_position_info, next_move_color, true, false);
        }
    }

    if (PositionInfo::get_castling_right(position_info, next_move_color, false))
    {
        if (target_square == QUEENSIDE_ROOK_START_SQUARE[next_move_color])
        {
            PositionInfo::set_castling_right(new_position_info, next_move_color, false, false);
        }
    }

    // TODO: fifty move rule
    // TODO: threefold

    previous_positions.push(position_info);
    position_info = new_position_info;
}

void Board::unmake_move(Move_t move)
{
    Square_t start_square = Move::get_start_square(move);
    Square_t target_square = Move::get_target_square(move);
    int move_flag = Move::get_flag(move);

    PositionInfo_t previous_position_info = previous_positions.top();
    previous_positions.pop();

    if (Move::is_pawn_promotion(move))
    {
        pieces[target_square] =
            Piece::create_piece(Piece::PAWN, PositionInfo::get_to_move(previous_position_info));
    }

    move_piece(target_square, start_square);

    bool is_capture = PositionInfo::get_captured_piece(position_info) != Piece::NONE;
    if (is_capture)
    {
        if (move_flag == Move::EN_PASSANT_FLAG)
        {
            pieces[get_en_passant_capture_square(start_square, target_square)] =
                PositionInfo::get_captured_piece(position_info);
        }
        else
        {
            pieces[target_square] = PositionInfo::get_captured_piece(position_info);
        }
    }

    if (move_flag == Move::CASTLE_FLAG)
    {
        if (target_square == start_square + 2)
        {
            // Short castle
            move_piece(start_square + 1, start_square + 3); // Moves rook
        }
        if (target_square == start_square - 2)
        {
            // Long castle
            move_piece(start_square - 1, start_square - 4); // Moves rook
        }
    }

    // TODO: fifty move rule
    // TODO: threefold

    position_info = previous_position_info;
}

void Board::reset()
{
    pieces.fill(Piece::NONE);
    position_info = 0;
    previous_positions = {};
}
