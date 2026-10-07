#include "move_generator.hpp"

#include "bitboard.hpp"
#include "board.hpp"
#include "directions.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "position_info.hpp"
#include "precomputations.hpp"

MoveGenerator::MoveGenerator(Board& board) : board(board)
{
}

MoveList MoveGenerator::get_moves()
{
    en_passant_square =
        Board::get_en_passant_square(position_info::get_en_passant_file(board.get_position_info()),
                                     position_info::get_to_move(board.get_position_info()));
    init_bitboards();
    MoveList moves;
    if (!is_double_check)
    {
        Bitboard moving_pieces_bb = friendly_pieces_bb;
        while (moving_pieces_bb != 0)
        {
            Square square = bitboard::pop_square(moving_pieces_bb);
            bitboard::set_all(pinned_piece_possible_squares_bb);
            if (bitboard::is_set(pinned_pieces_bb, square))
            {
                pinned_piece_possible_squares_bb = get_pinned_piece_possible_squares(square);
            }
            add_piece_moves(moves, square);
        }
    }
    else
    {
        add_piece_moves(moves, bitboard::get_square(friendly_king_bb));
    }
    return moves;
}

bool MoveGenerator::is_check() const
{
    return checking_piece_bb != 0;
}

int MoveGenerator::perft(int depth)
{
    MoveList moves = get_moves();
    if (depth == 1)
    {
        return moves.size();
    }

    int nodes = 0;
    for (Move move : moves)
    {
        board.make_move(move);
        int move_nodes = perft(depth - 1);
        nodes += move_nodes;
        board.unmake_move(move);
    }
    return nodes;
}

void MoveGenerator::init_bitboards()
{
    bitboard::clear_all(friendly_pieces_bb);
    bitboard::clear_all(enemy_pieces_bb);
    bitboard::clear_all(all_pieces_bb);
    bitboard::clear_all(friendly_king_bb);
    for (Square square = 0; square < square::TOTAL_SQUARES; square++)
    {
        if (board.is_occupied(square))
        {
            bitboard::set_square(all_pieces_bb, square);
            if (board.get_piece_color(square) ==
                position_info::get_to_move(board.get_position_info()))
            {
                bitboard::set_square(friendly_pieces_bb, square);
                if (board.get_piece_type(square) == piece::KING)
                {
                    bitboard::set_square(friendly_king_bb, square);
                }
            }
            else
            {
                bitboard::set_square(enemy_pieces_bb, square);
            }
        }
    }

    update_attacks();

    bitboard::clear_all(pinned_pieces_bb);
    bitboard::set_all(blocking_squares_bb);

    if (!is_double_check)
    {
        pinned_pieces_bb = get_pinned_pieces();
        if (checking_piece_bb != 0)
        {
            blocking_squares_bb = get_blocking_squares();
        }
    }
}

void MoveGenerator::add_piece_moves(MoveList& moves, Square square) const
{
    PieceType piece_type = board.get_piece_type(square);
    Color color = board.get_piece_color(square);
    switch (piece_type)
    {
    case piece::PAWN:
        add_pawn_moves(moves, square, color);
        break;
    case piece::KNIGHT:
        add_knight_moves(moves, square);
        break;
    case piece::BISHOP:
    case piece::ROOK:
    case piece::QUEEN:
        add_sliding_piece_moves(moves, square, piece_type);
        break;
    case piece::KING:
        add_king_moves(moves, square, color);
        break;
    default:
        break;
    }
}

void MoveGenerator::add_pawn_moves(MoveList& moves, Square square, Color color) const
{
    Direction direction = directions::pawn_directions[color];
    bool is_promotion = piece::can_pawn_promote(square, color);

    if (bitboard::is_clear(all_pieces_bb, square + direction))
    {
        if (get_legal_squares(bitboard::create_bitboard(square + direction)) != 0)
        {
            add_pawn_move(moves, square, square + direction, move::NO_FLAG, is_promotion);
        }
        if (piece::can_pawn_move_two_spaces(square, color) &&
            bitboard::is_clear(all_pieces_bb, square + (direction * 2)))
        {
            if (get_legal_squares(bitboard::create_bitboard(square + (direction * 2))) != 0)
            {
                moves.push_back(move::create_move(square, square + (direction * 2),
                                                  move::TWO_SPACE_PAWN_MOVE_FLAG));
            }
        }
    }

    Bitboard pseudo_legal_moves_bb =
        precomputations::pawn_attacks[color][square] & enemy_pieces_bb;
    Bitboard legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    while (legal_moves_bb != 0)
    {
        add_pawn_move(moves, square, bitboard::pop_square(legal_moves_bb), move::NO_FLAG,
                      is_promotion);
    }

    if (en_passant_square != -1 &&
        bitboard::is_set(precomputations::pawn_attacks[color][square], en_passant_square))
    {
        if (is_en_passant_legal(square, en_passant_square))
        {
            moves.push_back(move::create_move(square, en_passant_square, move::EN_PASSANT_FLAG));
        }
    }
}

void MoveGenerator::add_pawn_move(MoveList& moves, Square start_square, Square target_square,
                                  int flag, bool is_promotion)
{
    if (!is_promotion)
    {
        moves.push_back(move::create_move(start_square, target_square, flag));
        return;
    }
    moves.push_back(move::create_move(start_square, target_square, move::PROMOTE_TO_QUEEN_FLAG));
    moves.push_back(move::create_move(start_square, target_square, move::PROMOTE_TO_ROOK_FLAG));
    moves.push_back(move::create_move(start_square, target_square, move::PROMOTE_TO_BISHOP_FLAG));
    moves.push_back(move::create_move(start_square, target_square, move::PROMOTE_TO_KNIGHT_FLAG));
}

void MoveGenerator::add_knight_moves(MoveList& moves, Square square) const
{
    Bitboard pseudo_legal_moves_bb =
        precomputations::knight_moves[square] & (~friendly_pieces_bb);
    Bitboard legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    move::add_moves_from_bitboard(moves, square, legal_moves_bb);
}

void MoveGenerator::add_sliding_piece_moves(MoveList& moves, Square square, Piece piece) const
{
    Bitboard pseudo_legal_moves_bb = 0;

    int start_index = 0;
    int end_index = directions::sliding_directions.size() - 1;
    if (piece == piece::BISHOP)
    {
        start_index = directions::LAST_ORTHOGONAL_DIRECTION_INDEX + 1;
    }
    if (piece == piece::ROOK)
    {
        end_index = directions::LAST_ORTHOGONAL_DIRECTION_INDEX;
    }

    for (int direction_index = start_index; direction_index <= end_index; direction_index++)
    {
        Direction direction = directions::sliding_directions[direction_index];
        for (int i = 1; i <= precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square target_square = square + (direction * i);
            if (bitboard::is_set(all_pieces_bb, target_square))
            {
                if (bitboard::is_set(enemy_pieces_bb, target_square))
                {
                    bitboard::set_square(pseudo_legal_moves_bb, target_square);
                }
                break;
            }
            bitboard::set_square(pseudo_legal_moves_bb, target_square);
        }
    }
    Bitboard legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    move::add_moves_from_bitboard(moves, square, legal_moves_bb);
}

void MoveGenerator::add_king_moves(MoveList& moves, Square square, Color color) const
{
    Bitboard pseudo_legal_moves_bb = precomputations::king_moves[square] & (~friendly_pieces_bb);
    Bitboard legal_moves_bb = pseudo_legal_moves_bb & (~attacked_squares_bb);
    move::add_moves_from_bitboard(moves, square, legal_moves_bb);

    if (checking_piece_bb == 0)
    {
        if (position_info::get_castling_right(board.get_position_info(), color, true) &&
            bitboard::is_clear(all_pieces_bb, square + 1) &&
            bitboard::is_clear(all_pieces_bb, square + 2))
        {
            if (bitboard::is_clear(attacked_squares_bb, square + 1) &&
                bitboard::is_clear(attacked_squares_bb, square + 2))
            {
                moves.push_back(move::create_move(square, square + 2, move::CASTLE_FLAG));
            }
        }

        if (position_info::get_castling_right(board.get_position_info(), color, false) &&
            bitboard::is_clear(all_pieces_bb, square - 1) &&
            bitboard::is_clear(all_pieces_bb, square - 2) &&
            bitboard::is_clear(all_pieces_bb, square - 3))
        {
            if (bitboard::is_clear(attacked_squares_bb, square - 1) &&
                bitboard::is_clear(attacked_squares_bb, square - 2))
            {
                moves.push_back(move::create_move(square, square - 2, move::CASTLE_FLAG));
            }
        }
    }
}

void MoveGenerator::update_attacks()
{
    bitboard::clear_all(attacked_squares_bb);
    bitboard::clear_all(checking_piece_bb);
    is_double_check = false;

    Bitboard attacking_pieces = enemy_pieces_bb;
    while (attacking_pieces != 0)
    {
        Square attacker_square = bitboard::pop_square(attacking_pieces);
        Bitboard piece_attacks = get_piece_attacks(attacker_square);

        if ((piece_attacks & friendly_king_bb) != 0)
        {
            if (checking_piece_bb == 0)
            {
                bitboard::set_square(checking_piece_bb, attacker_square);
            }
            else
            {
                is_double_check = true;
            }
        }
        attacked_squares_bb |= piece_attacks;
    }
}

Bitboard MoveGenerator::get_piece_attacks(Square square) const
{
    Bitboard piece_attacks = 0;
    PieceType piece_type = board.get_piece_type(square);
    Color color = board.get_piece_color(square);
    switch (piece_type)
    {
    case piece::PAWN:
        piece_attacks = get_pawn_attacks(square, color);
        break;
    case piece::KNIGHT:
        piece_attacks = get_knight_attacks(square);
        break;
    case piece::BISHOP:
    case piece::ROOK:
    case piece::QUEEN:
        piece_attacks = get_sliding_piece_attacks(square, piece_type);
        break;
    case piece::KING:
        piece_attacks = get_king_attacks(square);
        break;
    default:
        piece_attacks = 0;
    }
    return piece_attacks;
}

Bitboard MoveGenerator::get_pawn_attacks(Square square, Color color)
{
    return precomputations::pawn_attacks[color][square];
}

Bitboard MoveGenerator::get_knight_attacks(Square square)
{
    return precomputations::knight_moves[square];
}

Bitboard MoveGenerator::get_sliding_piece_attacks(Square square, Piece piece) const
{
    Bitboard attacks = 0;

    int start_index = 0;
    int end_index = directions::sliding_directions.size() - 1;
    if (piece == piece::BISHOP)
    {
        start_index = directions::LAST_ORTHOGONAL_DIRECTION_INDEX + 1;
    }
    if (piece == piece::ROOK)
    {
        end_index = directions::LAST_ORTHOGONAL_DIRECTION_INDEX;
    }

    for (int direction_index = start_index; direction_index <= end_index; direction_index++)
    {
        Direction direction = directions::sliding_directions[direction_index];
        for (int i = 1; i <= precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square target_square = square + (direction * i);
            bitboard::set_square(attacks, target_square);
            if (bitboard::is_set(all_pieces_bb, target_square) &&
                bitboard::is_clear(friendly_king_bb, target_square))
            {
                break;
            }
        }
    }
    return attacks;
}

Bitboard MoveGenerator::get_king_attacks(Square square)
{
    return precomputations::king_moves[square];
}

Bitboard MoveGenerator::get_pinned_pieces() const
{
    Bitboard pinned_pieces = 0;
    Square square = bitboard::get_square(friendly_king_bb);
    for (Direction direction : directions::sliding_directions)
    {
        Square pinned_piece_square = -1;

        for (int i = 1; i <= precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square target_square = square + (direction * i);
            if (bitboard::is_set(friendly_pieces_bb, target_square))
            {
                if (pinned_piece_square == -1)
                {
                    pinned_piece_square = target_square;
                }
                else
                {
                    break;
                }
            }
            if (bitboard::is_set(enemy_pieces_bb, target_square))
            {
                if (pinned_piece_square != -1)
                {
                    if (piece::can_move_in_direction(board.get_piece_type(target_square),
                                                     -1 * direction))
                    {
                        bitboard::set_square(pinned_pieces, pinned_piece_square);
                    }
                }
                break;
            }
        }
    }
    return pinned_pieces;
}

Bitboard MoveGenerator::get_blocking_squares() const
{
    Bitboard blocking_squares = 0;
    Square king_square = bitboard::get_square(friendly_king_bb);
    Square checking_piece_square = bitboard::get_square(checking_piece_bb);
    if (!piece::is_sliding_piece(board.get_piece_type(checking_piece_square)))
    {
        return blocking_squares;
    }
    Direction direction = directions::get_ray_direction(king_square, checking_piece_square);
    Square target_square = king_square + direction;
    while (target_square != checking_piece_square)
    {
        bitboard::set_square(blocking_squares, target_square);
        target_square += direction;
    }

    return blocking_squares;
}

Bitboard MoveGenerator::get_pinned_piece_possible_squares(Square square) const
{
    Bitboard pinned_piece_possible_squares = 0;
    Square king_square = bitboard::get_square(friendly_king_bb);
    Direction direction = directions::get_ray_direction(king_square, square);
    Square target_square = king_square + direction;
    while (bitboard::is_clear(enemy_pieces_bb, target_square))
    {
        bitboard::set_square(pinned_piece_possible_squares, target_square);
        target_square += direction;
    }
    bitboard::set_square(pinned_piece_possible_squares, target_square);
    return pinned_piece_possible_squares;
}

Bitboard MoveGenerator::get_legal_squares(Bitboard moves) const
{
    return moves & (blocking_squares_bb | checking_piece_bb) & pinned_piece_possible_squares_bb;
}

bool MoveGenerator::is_en_passant_legal(Square start_square, Square target_square) const
{
    Square capture_square = Board::get_en_passant_capture_square(start_square, target_square);
    Square king_square = bitboard::get_square(friendly_king_bb);
    Direction direction = directions::get_ray_direction(king_square, start_square);

    if (direction == directions::WEST || direction == directions::EAST)
    {
        Bitboard en_passant_pieces_bb = all_pieces_bb;
        bitboard::clear_square(en_passant_pieces_bb, start_square);
        bitboard::clear_square(en_passant_pieces_bb, capture_square);

        for (int i = 1; i <= precomputations::get_squares_to_edge(king_square, direction); i++)
        {
            Square scan_square = king_square + (direction * i);
            if (bitboard::is_set(en_passant_pieces_bb, scan_square))
            {
                if (bitboard::is_set(enemy_pieces_bb, scan_square))
                {
                    Piece piece_type = board.get_piece_type(scan_square);
                    if (piece_type == piece::QUEEN || piece_type == piece::ROOK)
                    {
                        return false;
                    }
                }
                break;
            }
        }
    }
    if (bitboard::is_set(pinned_piece_possible_squares_bb, target_square))
    {
        if (checking_piece_bb == 0 || bitboard::is_set(checking_piece_bb, capture_square))
        {
            return true;
        }
    }
    return false;
}
