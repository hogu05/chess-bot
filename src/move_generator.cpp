#include "move_generator.hpp"

#include "bitboard.hpp"
#include "directions.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "position_info.hpp"
#include "precomputations.hpp"

MoveGenerator::MoveGenerator(Board &board) : board(board)
{
}

std::vector<Move_t> MoveGenerator::get_moves()
{
    en_passant_square =
        Board::get_en_passant_square(PositionInfo::get_en_passant_file(board.position_info),
                                     PositionInfo::get_to_move(board.position_info));
    init_bitboards();
    std::vector<Move_t> moves;
    if (!is_double_check)
    {
        Bitboard_t moving_pieces_bb = friendly_pieces_bb;
        while (moving_pieces_bb != 0)
        {
            Square_t square = Bitboard::pop_square(moving_pieces_bb);
            Bitboard::set_all(pinned_piece_possible_squares_bb);
            if (Bitboard::is_set(pinned_pieces_bb, square))
            {
                pinned_piece_possible_squares_bb = get_pinned_piece_possible_squares(square);
            }
            std::vector<Move_t> piece_moves = get_piece_moves(square);
            moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
        }
    }
    else
    {
        moves = get_piece_moves(Bitboard::get_square(friendly_king_bb));
    }
    return moves;
}

bool MoveGenerator::is_check()
{
    return checking_piece_bb != 0;
}

int MoveGenerator::perft(int depth)
{
    if (depth == 0)
    {
        return 1;
    }
    int nodes = 0;
    for (Move_t move : get_moves())
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
    Bitboard::clear_all(friendly_pieces_bb);
    Bitboard::clear_all(enemy_pieces_bb);
    Bitboard::clear_all(all_pieces_bb);
    Bitboard::clear_all(friendly_king_bb);
    for (Square_t square = 0; square < Board::TOTAL_SQUARES; square++)
    {
        if (board.is_occupied(square))
        {
            Bitboard::set_square(all_pieces_bb, square);
            if (board.get_piece_color(square) == PositionInfo::get_to_move(board.position_info))
            {
                Bitboard::set_square(friendly_pieces_bb, square);
                if (board.get_piece_type(square) == Piece::KING)
                {
                    Bitboard::set_square(friendly_king_bb, square);
                }
            }
            else
            {
                Bitboard::set_square(enemy_pieces_bb, square);
            }
        }
    }

    update_attacks();

    Bitboard::clear_all(pinned_pieces_bb);
    Bitboard::set_all(blocking_squares_bb);

    if (!is_double_check)
    {
        pinned_pieces_bb = get_pinned_pieces();
        if (checking_piece_bb != 0)
        {
            blocking_squares_bb = get_blocking_squares();
        }
    }
}

std::vector<Move_t> MoveGenerator::get_piece_moves(Square_t square)
{
    PieceType_t piece_type = board.get_piece_type(square);
    Color_t color = board.get_piece_color(square);
    switch (piece_type)
    {
    case Piece::PAWN:
        return get_pawn_moves(square, color);
    case Piece::KNIGHT:
        return get_knight_moves(square);
    case Piece::BISHOP:
    case Piece::ROOK:
    case Piece::QUEEN:
        return get_sliding_piece_moves(square, piece_type);
    case Piece::KING:
        return get_king_moves(square, color);
    default:
        return {};
    }
}

std::vector<Move_t> MoveGenerator::get_pawn_moves(Square_t square, Color_t color)
{
    std::vector<Move_t> moves;

    Direction_t direction = Directions::pawn_directions[color];

    if (Bitboard::is_clear(all_pieces_bb, square + direction))
    {
        if (get_legal_squares(Bitboard::create_bitboard(square + direction)) != 0)
        {
            moves.push_back(Move::create_move(square, square + direction, Move::NO_FLAG));
        }
        if (Piece::can_pawn_move_two_spaces(square, color) &&
            Bitboard::is_clear(all_pieces_bb, square + direction * 2))
        {
            if (get_legal_squares(Bitboard::create_bitboard(square + direction * 2)) != 0)
            {
                moves.push_back(Move::create_move(square, square + direction * 2,
                                                  Move::TWO_SPACE_PAWN_MOVE_FLAG));
            }
        }
    }

    Bitboard_t pseudo_legal_moves_bb =
        Precomputations::pawn_attacks[color][square] & enemy_pieces_bb;
    Bitboard_t legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    std::vector<Move_t> legal_moves = Move::create_moves_from_bitboard(square, legal_moves_bb);
    moves.insert(moves.end(), legal_moves.begin(), legal_moves.end());

    if (Bitboard::is_set(Precomputations::pawn_attacks[color][square], en_passant_square))
    {
        if (is_en_passant_legal(square, en_passant_square))
        {
            moves.push_back(Move::create_move(square, en_passant_square, Move::EN_PASSANT_FLAG));
        }
    }

    if (Piece::can_pawn_promote(square, color))
    {
        moves = get_pawn_promotion_moves(moves);
    }

    return moves;
}

std::vector<Move_t> MoveGenerator::get_pawn_promotion_moves(std::vector<Move_t> moves)
{
    std::vector<Move_t> promotion_moves;
    for (Move_t move : moves)
    {
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_KNIGHT_FLAG));
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_BISHOP_FLAG));
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_ROOK_FLAG));
        promotion_moves.push_back(Move::create_move(move, Move::PROMOTE_TO_QUEEN_FLAG));
    }
    return promotion_moves;
}

std::vector<Move_t> MoveGenerator::get_knight_moves(Square_t square)
{
    Bitboard_t pseudo_legal_moves_bb =
        Precomputations::knight_moves[square] & (~friendly_pieces_bb);
    Bitboard_t legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    return Move::create_moves_from_bitboard(square, legal_moves_bb);
}

std::vector<Move_t> MoveGenerator::get_sliding_piece_moves(Square_t square, Piece_t piece)
{
    Bitboard_t pseudo_legal_moves_bb = 0;

    int start_index = 0;
    int end_index = Directions::sliding_directions.size() - 1;
    if (piece == Piece::BISHOP)
    {
        start_index = Directions::LAST_ORTHOGONAL_DIRECTION_INDEX + 1;
    }
    if (piece == Piece::ROOK)
    {
        end_index = Directions::LAST_ORTHOGONAL_DIRECTION_INDEX;
    }

    for (int direction_index = start_index; direction_index <= end_index; direction_index++)
    {
        Direction_t direction = Directions::sliding_directions[direction_index];
        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square_t target_square = square + direction * i;
            if (Bitboard::is_set(all_pieces_bb, target_square))
            {
                if (Bitboard::is_set(enemy_pieces_bb, target_square))
                {
                    Bitboard::set_square(pseudo_legal_moves_bb, target_square);
                }
                break;
            }
            Bitboard::set_square(pseudo_legal_moves_bb, target_square);
        }
    }
    Bitboard_t legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    return Move::create_moves_from_bitboard(square, legal_moves_bb);
}

std::vector<Move_t> MoveGenerator::get_king_moves(Square_t square, Color_t color)
{
    Bitboard_t pseudo_legal_moves_bb = Precomputations::king_moves[square] & (~friendly_pieces_bb);
    Bitboard_t legal_moves_bb = pseudo_legal_moves_bb & (~attacked_squares_bb);
    std::vector<Move_t> moves = Move::create_moves_from_bitboard(square, legal_moves_bb);

    if (checking_piece_bb == 0)
    {
        if (PositionInfo::get_castling_right(board.position_info, color, true) &&
            Bitboard::is_clear(all_pieces_bb, square + 1) &&
            Bitboard::is_clear(all_pieces_bb, square + 2))
        {
            if (Bitboard::is_clear(attacked_squares_bb, square + 1) &&
                Bitboard::is_clear(attacked_squares_bb, square + 2))
            {
                moves.push_back(Move::create_move(square, square + 2, Move::CASTLE_FLAG));
            }
        }

        if (PositionInfo::get_castling_right(board.position_info, color, false) &&
            Bitboard::is_clear(all_pieces_bb, square - 1) &&
            Bitboard::is_clear(all_pieces_bb, square - 2) &&
            Bitboard::is_clear(all_pieces_bb, square - 3))
        {
            if (Bitboard::is_clear(attacked_squares_bb, square - 1) &&
                Bitboard::is_clear(attacked_squares_bb, square - 2))
            {
                moves.push_back(Move::create_move(square, square - 2, Move::CASTLE_FLAG));
            }
        }
    }

    return moves;
}

void MoveGenerator::update_attacks()
{
    Bitboard::clear_all(attacked_squares_bb);
    Bitboard::clear_all(checking_piece_bb);
    is_double_check = false;

    Bitboard_t attacking_pieces = enemy_pieces_bb;
    while (attacking_pieces != 0)
    {
        Square_t attacker_square = Bitboard::pop_square(attacking_pieces);
        Bitboard_t piece_attacks = get_piece_attacks(attacker_square);

        if ((piece_attacks & friendly_king_bb) != 0)
        {
            if (checking_piece_bb == 0)
            {
                Bitboard::set_square(checking_piece_bb, attacker_square);
            }
            else
            {
                is_double_check = true;
            }
        }
        attacked_squares_bb |= piece_attacks;
    }
}

Bitboard_t MoveGenerator::get_piece_attacks(Square_t square)
{
    Bitboard_t piece_attacks = 0;
    PieceType_t piece_type = board.get_piece_type(square);
    Color_t color = board.get_piece_color(square);
    switch (piece_type)
    {
    case Piece::PAWN:
        piece_attacks = get_pawn_attacks(square, color);
        break;
    case Piece::KNIGHT:
        piece_attacks = get_knight_attacks(square);
        break;
    case Piece::BISHOP:
    case Piece::ROOK:
    case Piece::QUEEN:
        piece_attacks = get_sliding_piece_attacks(square, piece_type);
        break;
    case Piece::KING:
        piece_attacks = get_king_attacks(square);
        break;
    default:
        piece_attacks = 0;
    }
    return piece_attacks;
}

Bitboard_t MoveGenerator::get_pawn_attacks(Square_t square, Color_t color)
{
    return Precomputations::pawn_attacks[color][square];
}

Bitboard_t MoveGenerator::get_knight_attacks(Square_t square)
{
    return Precomputations::knight_moves[square];
}

Bitboard_t MoveGenerator::get_sliding_piece_attacks(Square_t square, Piece_t piece)
{
    Bitboard_t attacks = 0;

    int start_index = 0;
    int end_index = Directions::sliding_directions.size() - 1;
    if (piece == Piece::BISHOP)
    {
        start_index = Directions::LAST_ORTHOGONAL_DIRECTION_INDEX + 1;
    }
    if (piece == Piece::ROOK)
    {
        end_index = Directions::LAST_ORTHOGONAL_DIRECTION_INDEX;
    }

    for (int direction_index = start_index; direction_index <= end_index; direction_index++)
    {
        Direction_t direction = Directions::sliding_directions[direction_index];
        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square_t target_square = square + direction * i;
            Bitboard::set_square(attacks, target_square);
            if (Bitboard::is_set(all_pieces_bb, target_square) &&
                Bitboard::is_clear(friendly_king_bb, target_square))
            {
                break;
            }
        }
    }
    return attacks;
}

Bitboard_t MoveGenerator::get_king_attacks(Square_t square)
{
    return Precomputations::king_moves[square];
}

Bitboard_t MoveGenerator::get_pinned_pieces()
{
    Bitboard_t pinned_pieces = 0;
    Square_t square = Bitboard::get_square(friendly_king_bb);
    for (Direction_t direction : Directions::sliding_directions)
    {
        Square_t pinned_piece_square = -1;

        for (int i = 1; i <= Precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square_t target_square = square + direction * i;
            if (Bitboard::is_set(friendly_pieces_bb, target_square))
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
            if (Bitboard::is_set(enemy_pieces_bb, target_square))
            {
                if (pinned_piece_square != -1)
                {
                    if (Piece::can_move_in_direction(board.get_piece_type(target_square),
                                                     -1 * direction))
                    {
                        Bitboard::set_square(pinned_pieces, pinned_piece_square);
                    }
                }
                break;
            }
        }
    }
    return pinned_pieces;
}

Bitboard_t MoveGenerator::get_blocking_squares()
{
    Bitboard_t blocking_squares = 0;
    Square_t king_square = Bitboard::get_square(friendly_king_bb);
    Square_t checking_piece_square = Bitboard::get_square(checking_piece_bb);
    if (!Piece::is_sliding_piece(board.get_piece_type(checking_piece_square)))
    {
        return blocking_squares;
    }
    Bitboard::clear_all(blocking_squares_bb);
    Direction_t direction = Directions::get_ray_direction(king_square, checking_piece_square);
    Square_t target_square = king_square + direction;
    while (target_square != checking_piece_square)
    {
        Bitboard::set_square(blocking_squares, target_square);
        target_square += direction;
    }

    return blocking_squares;
}

Bitboard_t MoveGenerator::get_pinned_piece_possible_squares(Square_t square)
{
    Bitboard_t pinned_piece_possible_squares = 0;
    Square_t king_square = Bitboard::get_square(friendly_king_bb);
    Direction_t direction = Directions::get_ray_direction(king_square, square);
    Square_t target_square = king_square + direction;
    while (Bitboard::is_clear(enemy_pieces_bb, target_square))
    {
        Bitboard::set_square(pinned_piece_possible_squares, target_square);
        target_square += direction;
    }
    Bitboard::set_square(pinned_piece_possible_squares, target_square);
    return pinned_piece_possible_squares;
}

Bitboard_t MoveGenerator::get_legal_squares(Bitboard_t moves)
{
    return moves & (blocking_squares_bb | checking_piece_bb) & pinned_piece_possible_squares_bb;
}

bool MoveGenerator::is_en_passant_legal(Square_t start_square, Square_t target_square)
{
    Square_t capture_square = Board::get_en_passant_capture_square(start_square, target_square);
    Square_t king_square = Bitboard::get_square(friendly_king_bb);
    Direction_t direction = Directions::get_ray_direction(king_square, start_square);

    if (direction == Directions::WEST || direction == Directions::EAST)
    {
        Bitboard_t en_passant_pieces_bb = all_pieces_bb;
        Bitboard::clear_square(en_passant_pieces_bb, start_square);
        Bitboard::clear_square(en_passant_pieces_bb, capture_square);

        for (int i = 1; i <= Precomputations::get_squares_to_edge(king_square, direction); i++)
        {
            Square_t scan_square = king_square + direction * i;
            if (Bitboard::is_set(en_passant_pieces_bb, scan_square))
            {
                if (Bitboard::is_set(enemy_pieces_bb, scan_square))
                {
                    Piece_t piece_type = board.get_piece_type(scan_square);
                    if (piece_type == Piece::QUEEN || piece_type == Piece::ROOK)
                    {
                        return false;
                    }
                }
                break;
            }
        }
    }
    if (Bitboard::is_set(pinned_piece_possible_squares_bb, target_square))
    {
        if (checking_piece_bb == 0 || Bitboard::is_set(checking_piece_bb, capture_square))
        {
            return true;
        }
    }
    return false;
}
