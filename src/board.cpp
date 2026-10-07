#include "board.hpp"

#include <sstream>
#include <string>

#include "hasher.hpp"
#include "move.hpp"
#include "notation.hpp"
#include "piece.hpp"
#include "position_info.hpp"

Square Board::get_en_passant_square(int en_passant_file, Color to_move)
{
    if (en_passant_file == -1)
    {
        return -1;
    }
    int rank = to_move == piece::WHITE ? 5 : 2;
    return square::create_square(en_passant_file, rank);
}

Square Board::get_en_passant_capture_square(Square start_square, Square target_square)
{
    int file = square::get_file(target_square);
    int rank = square::get_rank(start_square);
    return square::create_square(file, rank);
}

void Board::load_position(const std::string& fen)
{
    reset();

    std::istringstream stream(fen);
    std::array<std::string, 6> fen_info;
    stream >> fen_info[0] >> fen_info[1] >> fen_info[2] >> fen_info[3] >> fen_info[4] >>
        fen_info[5];

    // Getting pieces
    Square square = 56;
    for (char current_char : fen_info[0])
    {
        if (current_char == '/')
        {
            square -= 16;
            continue;
        }
        if (std::isdigit(current_char) != 0)
        {
            square += current_char - '0';
        }
        else
        {
            pieces[square] = notation::get_piece_from_letter(current_char);
            hasher::update_square(hash, square, pieces[square]);
            square++;
        }
    }

    // Getting to_move color
    if (fen_info[1] == "w")
    {
        position_info::set_to_move(position_info, piece::WHITE);
    }

    if (fen_info[1] == "b")
    {
        position_info::set_to_move(position_info, piece::BLACK);
    }

    // Getting castling rights
    for (char current_char : fen_info[2])
    {
        switch (current_char)
        {
        case 'K':
            position_info::set_castling_right(position_info, piece::WHITE, true, true);
            break;
        case 'Q':
            position_info::set_castling_right(position_info, piece::WHITE, false, true);
            break;
        case 'k':
            position_info::set_castling_right(position_info, piece::BLACK, true, true);
            break;
        case 'q':
            position_info::set_castling_right(position_info, piece::BLACK, false, true);
            break;
        default:
            break;
        }
    }

    // Getting en passant
    if (!fen_info[3].empty() && fen_info[3] != "-")
    {
        position_info::set_en_passant(position_info, true, fen_info[3][0] - 'a');
    }

    // Getting fifty move rule half-moves
    if (!fen_info[4].empty())
    {
        position_info::set_fifty_move_ply(position_info, std::stoi(fen_info[4]));
    }

    // Getting full moves
    if (!fen_info[5].empty())
    {
        // Full moves of the game
    }

    previous_positions.push(position_info);

    hasher::update_to_move(hash, position_info::get_to_move(position_info));
    hasher::update_castling_rights(hash, position_info);
    hasher::update_en_passant(hash, position_info::get_en_passant_file(position_info));
    hash_count[hash]++;
}

bool Board::is_occupied(Square square) const
{
    return piece::get_piece_type(pieces[square]) != piece::NONE;
}

Color Board::get_piece_color(Square square) const
{
    return piece::get_piece_color(pieces[square]);
}

PieceType Board::get_piece_type(Square square) const
{
    return piece::get_piece_type(pieces[square]);
}

void Board::move_piece(Square start_square, Square target_square)
{
    pieces[target_square] = pieces[start_square];
    pieces[start_square] = piece::NONE;
}

void Board::make_move(Move move)
{
    previous_hashes.push(hash);

    PositionInfo new_position_info = 0;
    position_info::set_castling_rights(new_position_info,
                                       position_info::get_castling_rights(position_info));
    Square start_square = move::get_start_square(move);
    Square target_square = move::get_target_square(move);
    int move_flag = move::get_flag(move);
    Color moving_color = position_info::get_to_move(position_info);
    Color next_move_color = piece::get_other_color(moving_color);

    position_info::set_to_move(new_position_info, next_move_color);

    position_info::set_captured_piece(new_position_info, get_captured_piece(move));

    if (move_flag == move::EN_PASSANT_FLAG)
    {
        Square en_passant_capture_square =
            get_en_passant_capture_square(start_square, target_square);
        hasher::update_square(hash, en_passant_capture_square, pieces[en_passant_capture_square]);
        pieces[en_passant_capture_square] = piece::NONE;
    }

    if (move_flag == move::TWO_SPACE_PAWN_MOVE_FLAG)
    {
        position_info::set_en_passant(new_position_info, true, square::get_file(start_square));
    }

    if (move_flag == move::CASTLE_FLAG)
    {
        if (target_square == start_square + 2)
        {
            // Short castle
            hasher::update_square(hash, start_square + 3, pieces[start_square + 3]);
            move_piece(start_square + 3, start_square + 1); // Moves rook
            hasher::update_square(hash, start_square + 1, pieces[start_square + 1]);
        }
        if (target_square == start_square - 2)
        {
            // Long castle
            hasher::update_square(hash, start_square - 4, pieces[start_square - 4]);
            move_piece(start_square - 4, start_square - 1); // Moves rook
            hasher::update_square(hash, start_square - 1, pieces[start_square - 1]);
        }
        position_info::set_castling_right(new_position_info, moving_color, true, false);
        position_info::set_castling_right(new_position_info, moving_color, false, false);
    }

    hasher::update_square(hash, target_square, pieces[target_square]);
    hasher::update_square(hash, start_square, pieces[start_square]);
    move_piece(start_square, target_square);

    if (move::is_pawn_promotion(move))
    {
        PieceType promoted_piece_type = move::get_pawn_promotion_piece_type(move);
        pieces[target_square] = piece::create_piece(promoted_piece_type, moving_color);
    }

    hasher::update_square(hash, target_square, pieces[target_square]);

    if (position_info::get_castling_right(position_info, moving_color, true))
    {
        if (start_square == KINGSIDE_ROOK_START_SQUARE[moving_color] ||
            start_square == KING_START_SQUARE[moving_color])
        {
            position_info::set_castling_right(new_position_info, moving_color, true, false);
        }
    }

    if (position_info::get_castling_right(position_info, moving_color, false))
    {
        if (start_square == QUEENSIDE_ROOK_START_SQUARE[moving_color] ||
            start_square == KING_START_SQUARE[moving_color])
        {
            position_info::set_castling_right(new_position_info, moving_color, false, false);
        }
    }

    if (position_info::get_castling_right(position_info, next_move_color, true))
    {
        if (target_square == KINGSIDE_ROOK_START_SQUARE[next_move_color])
        {
            position_info::set_castling_right(new_position_info, next_move_color, true, false);
        }
    }

    if (position_info::get_castling_right(position_info, next_move_color, false))
    {
        if (target_square == QUEENSIDE_ROOK_START_SQUARE[next_move_color])
        {
            position_info::set_castling_right(new_position_info, next_move_color, false, false);
        }
    }

    hasher::update_to_move(hash, position_info::get_to_move(new_position_info));

    if (position_info::get_castling_rights(new_position_info) !=
        position_info::get_castling_rights(position_info))
    {
        hasher::update_castling_rights(hash, new_position_info);
    }

    if (position_info::get_en_passant_file(new_position_info) !=
        position_info::get_en_passant_file(position_info))
    {
        hasher::update_en_passant(hash, position_info::get_en_passant_file(new_position_info));
    }
    hash_count[hash]++;

    previous_positions.push(position_info);
    position_info = new_position_info;
}

void Board::unmake_move(Move move)
{
    Square start_square = move::get_start_square(move);
    Square target_square = move::get_target_square(move);
    int move_flag = move::get_flag(move);

    PositionInfo previous_position_info = previous_positions.top();
    previous_positions.pop();

    hash_count[hash]--;
    hash = previous_hashes.top();
    previous_hashes.pop();

    if (move::is_pawn_promotion(move))
    {
        pieces[target_square] =
            piece::create_piece(piece::PAWN, position_info::get_to_move(previous_position_info));
    }

    move_piece(target_square, start_square);

    bool is_capture = position_info::get_captured_piece(position_info) != piece::NONE;
    if (is_capture)
    {
        if (move_flag == move::EN_PASSANT_FLAG)
        {
            pieces[get_en_passant_capture_square(start_square, target_square)] =
                position_info::get_captured_piece(position_info);
        }
        else
        {
            pieces[target_square] = position_info::get_captured_piece(position_info);
        }
    }

    if (move_flag == move::CASTLE_FLAG)
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
    position_info = previous_position_info;
}

void Board::reset()
{
    pieces.fill(piece::NONE);
    position_info = 0;
    previous_positions = {};
    hash = 0;
    previous_hashes = {};
    hash_count.clear();
}

Piece Board::get_captured_piece(Move move) const
{
    Square target_square = move::get_target_square(move);
    if (move::get_flag(move) == move::EN_PASSANT_FLAG)
    {
        Square capture_square =
            get_en_passant_capture_square(move::get_start_square(move), target_square);
        return pieces[capture_square];
    }
    return pieces[target_square];
}

const std::array<Piece, square::TOTAL_SQUARES>& Board::get_pieces() const
{
    return pieces;
}

PositionInfo Board::get_position_info() const
{
    return position_info;
}

bool Board::is_threefold_repetition() const
{
    auto it = hash_count.find(hash);
    return it != hash_count.end() && it->second >= 3;
}
