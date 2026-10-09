#include "board.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

#include "bitboard.hpp"
#include "color.hpp"
#include "hasher.hpp"
#include "move.hpp"
#include "notation.hpp"
#include "piece.hpp"
#include "position_info.hpp"

void Board::load_position(const std::string& fen)
{
    reset();

    std::istringstream stream(fen);
    std::array<std::string, 6> fen_info;
    stream >> fen_info[0] >> fen_info[1] >> fen_info[2] >> fen_info[3] >> fen_info[4] >>
        fen_info[5];

    Square square = square::a8;
    for (const char current_char : fen_info[0])
    {
        if (current_char == '/')
        {
            square -= 2 * square::FILES;
            continue;
        }
        if (std::isdigit(current_char) != 0)
        {
            square += current_char - '0';
        }
        else
        {
            const Piece piece = notation::get_piece_from_letter(current_char);
            place_piece(square, piece);
            hasher::update_square(hash, square, piece);
            square++;
        }
    }

    if (fen_info[1] == "w")
    {
        position_info::set_to_move(position_info, color::WHITE);
    }

    if (fen_info[1] == "b")
    {
        position_info::set_to_move(position_info, color::BLACK);
    }

    for (const char current_char : fen_info[2])
    {
        switch (current_char)
        {
        case 'K':
            position_info::set_castling_right(position_info, color::WHITE, true, true);
            break;
        case 'Q':
            position_info::set_castling_right(position_info, color::WHITE, false, true);
            break;
        case 'k':
            position_info::set_castling_right(position_info, color::BLACK, true, true);
            break;
        case 'q':
            position_info::set_castling_right(position_info, color::BLACK, false, true);
            break;
        default:
            break;
        }
    }

    if (!fen_info[3].empty() && fen_info[3] != "-")
    {
        position_info::set_en_passant(position_info, true, fen_info[3][0] - 'a');
    }

    if (!fen_info[4].empty())
    {
        position_info::set_fifty_move_ply(position_info, std::stoi(fen_info[4]));
    }

    if (get_to_move() == color::BLACK)
    {
        hasher::update_to_move(hash);
    }
    hasher::update_castling_rights(hash, position_info);
    hasher::update_en_passant(hash, position_info::get_en_passant_file(position_info));
}

void Board::make_move(Move move)
{
    previous_hashes[ply] = hash;
    previous_positions[ply] = position_info;
    ply++;

    const Square start_square = move::get_start_square(move);
    const Square target_square = move::get_target_square(move);
    const int move_flag = move::get_flag(move);
    const Color moving_color = get_to_move();
    const Color next_move_color = color::get_other_color(moving_color);
    const Piece captured_piece = get_captured_piece(move);

    PositionInfo new_position_info = 0;
    position_info::set_castling_rights(new_position_info,
                                       position_info::get_castling_rights(position_info));
    position_info::set_to_move(new_position_info, next_move_color);
    position_info::set_captured_piece(new_position_info, captured_piece);
    position_info::set_fifty_move_ply(new_position_info,
                                      get_piece_type(start_square) == piece::PAWN ||
                                              captured_piece != piece::NONE
                                          ? 0
                                          : get_fifty_move_ply() + 1);

    if (move_flag == move::EN_PASSANT_FLAG)
    {
        const Square en_passant_capture_square =
            get_en_passant_capture_square(start_square, target_square);
        hasher::update_square(hash, en_passant_capture_square, squares[en_passant_capture_square]);
        remove_piece(en_passant_capture_square);
    }

    if (move_flag == move::TWO_SPACE_PAWN_MOVE_FLAG)
    {
        position_info::set_en_passant(new_position_info, true, square::get_file(start_square));
    }

    if (move_flag == move::CASTLE_FLAG)
    {
        if (target_square == start_square + 2)
        {
            hasher::update_square(hash, start_square + 3, squares[start_square + 3]);
            move_piece(start_square + 3, start_square + 1);
            hasher::update_square(hash, start_square + 1, squares[start_square + 1]);
        }
        if (target_square == start_square - 2)
        {
            hasher::update_square(hash, start_square - 4, squares[start_square - 4]);
            move_piece(start_square - 4, start_square - 1);
            hasher::update_square(hash, start_square - 1, squares[start_square - 1]);
        }
        position_info::set_castling_right(new_position_info, moving_color, true, false);
        position_info::set_castling_right(new_position_info, moving_color, false, false);
    }

    hasher::update_square(hash, target_square, squares[target_square]);
    hasher::update_square(hash, start_square, squares[start_square]);
    move_piece(start_square, target_square);

    if (move::is_pawn_promotion(move))
    {
        remove_piece(target_square);
        place_piece(target_square,
                    piece::create_piece(move::get_pawn_promotion_piece_type(move), moving_color));
    }

    hasher::update_square(hash, target_square, squares[target_square]);

    if (start_square == KING_START_SQUARE[moving_color] ||
        start_square == KINGSIDE_ROOK_START_SQUARE[moving_color])
    {
        position_info::set_castling_right(new_position_info, moving_color, true, false);
    }

    if (start_square == KING_START_SQUARE[moving_color] ||
        start_square == QUEENSIDE_ROOK_START_SQUARE[moving_color])
    {
        position_info::set_castling_right(new_position_info, moving_color, false, false);
    }

    if (target_square == KINGSIDE_ROOK_START_SQUARE[next_move_color])
    {
        position_info::set_castling_right(new_position_info, next_move_color, true, false);
    }

    if (target_square == QUEENSIDE_ROOK_START_SQUARE[next_move_color])
    {
        position_info::set_castling_right(new_position_info, next_move_color, false, false);
    }

    hasher::update_to_move(hash);

    if (position_info::get_castling_rights(new_position_info) !=
        position_info::get_castling_rights(position_info))
    {
        hasher::update_castling_rights(hash, position_info);
        hasher::update_castling_rights(hash, new_position_info);
    }

    if (position_info::get_en_passant_file(new_position_info) !=
        position_info::get_en_passant_file(position_info))
    {
        hasher::update_en_passant(hash, position_info::get_en_passant_file(position_info));
        hasher::update_en_passant(hash, position_info::get_en_passant_file(new_position_info));
    }
    position_info = new_position_info;
}

void Board::unmake_move(Move move)
{
    const Square start_square = move::get_start_square(move);
    const Square target_square = move::get_target_square(move);
    const int move_flag = move::get_flag(move);
    const Piece captured_piece = position_info::get_captured_piece(position_info);

    ply--;
    const PositionInfo previous_position_info = previous_positions[ply];
    hash = previous_hashes[ply];

    if (move::is_pawn_promotion(move))
    {
        remove_piece(target_square);
        place_piece(target_square, piece::create_piece(piece::PAWN, position_info::get_to_move(
                                                                        previous_position_info)));
    }

    move_piece(target_square, start_square);

    if (captured_piece != piece::NONE)
    {
        if (move_flag == move::EN_PASSANT_FLAG)
        {
            place_piece(get_en_passant_capture_square(start_square, target_square), captured_piece);
        }
        else
        {
            place_piece(target_square, captured_piece);
        }
    }

    if (move_flag == move::CASTLE_FLAG)
    {
        if (target_square == start_square + 2)
        {
            move_piece(start_square + 1, start_square + 3);
        }
        if (target_square == start_square - 2)
        {
            move_piece(start_square - 1, start_square - 4);
        }
    }
    position_info = previous_position_info;
}

void Board::make_null_move()
{
    previous_hashes[ply] = hash;
    previous_positions[ply] = position_info;
    ply++;

    PositionInfo new_position_info = 0;
    position_info::set_castling_rights(new_position_info,
                                       position_info::get_castling_rights(position_info));
    position_info::set_to_move(new_position_info, color::get_other_color(get_to_move()));

    hasher::update_to_move(hash);
    hasher::update_en_passant(hash, position_info::get_en_passant_file(position_info));
    hasher::update_en_passant(hash, position_info::get_en_passant_file(new_position_info));
    position_info = new_position_info;
}

void Board::unmake_null_move()
{
    ply--;
    position_info = previous_positions[ply];
    hash = previous_hashes[ply];
}

bool Board::is_repetition(int root_ply) const
{
    int game_repetitions = 0;
    for (int i = ply - 2; i >= std::max(ply - get_fifty_move_ply(), 0); i -= 2)
    {
        if (previous_hashes[i] != hash)
        {
            continue;
        }
        if (i >= root_ply)
        {
            return true;
        }
        game_repetitions++;
        if (game_repetitions == 2)
        {
            return true;
        }
    }
    return false;
}

void Board::place_piece(Square square, Piece piece)
{
    squares[square] = piece;
    bitboard::set_square(pieces_by_type[piece::get_piece_type(piece)], square);
    bitboard::set_square(pieces_by_color[piece::get_piece_color(piece)], square);
}

void Board::remove_piece(Square square)
{
    const Piece piece = squares[square];
    bitboard::clear_square(pieces_by_type[piece::get_piece_type(piece)], square);
    bitboard::clear_square(pieces_by_color[piece::get_piece_color(piece)], square);
    squares[square] = piece::NONE;
}

void Board::move_piece(Square start_square, Square target_square)
{
    remove_piece(target_square);
    place_piece(target_square, squares[start_square]);
    remove_piece(start_square);
}

void Board::reset()
{
    squares.fill(piece::NONE);
    pieces_by_type.fill(0);
    pieces_by_color.fill(0);
    position_info = 0;
    hash = 0;
    ply = 0;
}
