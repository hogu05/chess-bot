#include "move_generator.hpp"

#include <bit>

#include "bitboard.hpp"
#include "board.hpp"
#include "color.hpp"
#include "directions.hpp"
#include "magic.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "precomputations.hpp"

MoveGenerator::MoveGenerator(Board& board) : board(board)
{
}

Bitboard MoveGenerator::get_friendly_pieces_bb() const
{
    return board.get_color_bb(board.get_to_move());
}

Bitboard MoveGenerator::get_enemy_pieces_bb() const
{
    return board.get_color_bb(color::get_other_color(board.get_to_move()));
}

Square MoveGenerator::get_friendly_king_square() const
{
    return board.get_king_square(board.get_to_move());
}

MoveList MoveGenerator::get_moves()
{
    init_bitboards();
    MoveList moves;
    Color color = board.get_to_move();

    add_king_moves(moves, get_friendly_king_square(), color);
    if (is_double_check)
    {
        return moves;
    }

    Bitboard pawns = board.get_piece_bb(piece::PAWN, color);
    while (pawns != 0)
    {
        Square square = bitboard::pop_square(pawns);
        update_pinned_piece_possible_squares(square);
        add_pawn_moves(moves, square, color);
    }

    Bitboard knights = board.get_piece_bb(piece::KNIGHT, color);
    while (knights != 0)
    {
        Square square = bitboard::pop_square(knights);
        update_pinned_piece_possible_squares(square);
        add_knight_moves(moves, square);
    }

    for (PieceType piece_type : {piece::BISHOP, piece::ROOK, piece::QUEEN})
    {
        Bitboard sliders = board.get_piece_bb(piece_type, color);
        while (sliders != 0)
        {
            Square square = bitboard::pop_square(sliders);
            update_pinned_piece_possible_squares(square);
            add_sliding_piece_moves(moves, square, piece_type);
        }
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

void MoveGenerator::update_pinned_piece_possible_squares(Square square)
{
    bitboard::set_all(pinned_piece_possible_squares_bb);
    if (bitboard::is_set(pinned_pieces_bb, square))
    {
        pinned_piece_possible_squares_bb = get_pinned_piece_possible_squares(square);
    }
}

void MoveGenerator::add_pawn_moves(MoveList& moves, Square square, Color color) const
{
    Bitboard all_pieces_bb = board.get_all_pieces_bb();
    Square en_passant_square = board.get_en_passant_square();

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
        precomputations::pawn_attacks[color][square] & get_enemy_pieces_bb();
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
        precomputations::knight_moves[square] & (~get_friendly_pieces_bb());
    Bitboard legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    move::add_moves_from_bitboard(moves, square, legal_moves_bb);
}

void MoveGenerator::add_sliding_piece_moves(MoveList& moves, Square square,
                                            PieceType piece_type) const
{
    Bitboard pseudo_legal_moves_bb =
        magic::get_slider_attacks(square, piece_type, board.get_all_pieces_bb()) &
        ~get_friendly_pieces_bb();
    Bitboard legal_moves_bb = get_legal_squares(pseudo_legal_moves_bb);
    move::add_moves_from_bitboard(moves, square, legal_moves_bb);
}

void MoveGenerator::add_king_moves(MoveList& moves, Square square, Color color) const
{
    Bitboard all_pieces_bb = board.get_all_pieces_bb();

    Bitboard pseudo_legal_moves_bb =
        precomputations::king_moves[square] & (~get_friendly_pieces_bb());
    Bitboard legal_moves_bb = pseudo_legal_moves_bb & (~attacked_squares_bb);
    move::add_moves_from_bitboard(moves, square, legal_moves_bb);

    if (checking_piece_bb == 0)
    {
        if (board.get_castling_right(color, true) &&
            bitboard::is_clear(all_pieces_bb, square + 1) &&
            bitboard::is_clear(all_pieces_bb, square + 2))
        {
            if (bitboard::is_clear(attacked_squares_bb, square + 1) &&
                bitboard::is_clear(attacked_squares_bb, square + 2))
            {
                moves.push_back(move::create_move(square, square + 2, move::CASTLE_FLAG));
            }
        }

        if (board.get_castling_right(color, false) &&
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
    Color friendly_color = board.get_to_move();
    Color enemy_color = color::get_other_color(friendly_color);
    Square king_square = get_friendly_king_square();

    Bitboard enemy_pawns = board.get_piece_bb(piece::PAWN, enemy_color);
    Bitboard enemy_knights = board.get_piece_bb(piece::KNIGHT, enemy_color);

    checking_piece_bb = (get_pawn_attacks(king_square, friendly_color) & enemy_pawns) |
                        (get_knight_attacks(king_square) & enemy_knights);
    attacked_squares_bb = get_king_attacks(board.get_king_square(enemy_color));

    while (enemy_pawns != 0)
    {
        attacked_squares_bb |= get_pawn_attacks(bitboard::pop_square(enemy_pawns), enemy_color);
    }

    while (enemy_knights != 0)
    {
        attacked_squares_bb |= get_knight_attacks(bitboard::pop_square(enemy_knights));
    }

    for (PieceType piece_type : {piece::BISHOP, piece::ROOK, piece::QUEEN})
    {
        Bitboard sliders = board.get_piece_bb(piece_type, enemy_color);
        while (sliders != 0)
        {
            Square slider_square = bitboard::pop_square(sliders);
            Bitboard slider_attacks = get_sliding_piece_attacks(slider_square, piece_type);
            if (bitboard::is_set(slider_attacks, king_square))
            {
                bitboard::set_square(checking_piece_bb, slider_square);
            }
            attacked_squares_bb |= slider_attacks;
        }
    }

    is_double_check = std::popcount(checking_piece_bb) > 1;
}

Bitboard MoveGenerator::get_pawn_attacks(Square square, Color color)
{
    return precomputations::pawn_attacks[color][square];
}

Bitboard MoveGenerator::get_knight_attacks(Square square)
{
    return precomputations::knight_moves[square];
}

Bitboard MoveGenerator::get_sliding_piece_attacks(Square square, PieceType piece_type) const
{
    return magic::get_slider_attacks(square, piece_type,
                                     board.get_all_pieces_bb() &
                                         ~bitboard::create_bitboard(get_friendly_king_square()));
}

Bitboard MoveGenerator::get_king_attacks(Square square)
{
    return precomputations::king_moves[square];
}

Bitboard MoveGenerator::get_pinned_pieces() const
{
    Square king_square = get_friendly_king_square();
    Color enemy_color = color::get_other_color(board.get_to_move());
    Bitboard enemy_queens = board.get_piece_bb(piece::QUEEN, enemy_color);

    Bitboard snipers =
        (magic::get_slider_attacks(king_square, piece::ROOK, get_enemy_pieces_bb()) &
         (board.get_piece_bb(piece::ROOK, enemy_color) | enemy_queens)) |
        (magic::get_slider_attacks(king_square, piece::BISHOP, get_enemy_pieces_bb()) &
         (board.get_piece_bb(piece::BISHOP, enemy_color) | enemy_queens));

    Bitboard pinned_pieces = 0;
    while (snipers != 0)
    {
        Bitboard blockers = precomputations::between[king_square][bitboard::pop_square(snipers)] &
                            board.get_all_pieces_bb();
        if (std::popcount(blockers) == 1)
        {
            pinned_pieces |= blockers;
        }
    }
    return pinned_pieces;
}

Bitboard MoveGenerator::get_blocking_squares() const
{
    return precomputations::between[get_friendly_king_square()]
                                   [bitboard::get_square(checking_piece_bb)];
}

Bitboard MoveGenerator::get_pinned_piece_possible_squares(Square square) const
{
    return precomputations::line[get_friendly_king_square()][square];
}

Bitboard MoveGenerator::get_legal_squares(Bitboard moves) const
{
    return moves & (blocking_squares_bb | checking_piece_bb) & pinned_piece_possible_squares_bb;
}

bool MoveGenerator::is_en_passant_legal(Square start_square, Square target_square) const
{
    Bitboard captured_pawn_bb = bitboard::create_bitboard(
        Board::get_en_passant_capture_square(start_square, target_square));
    Square king_square = get_friendly_king_square();
    Color enemy_color = color::get_other_color(board.get_to_move());
    Bitboard enemy_queens = board.get_piece_bb(piece::QUEEN, enemy_color);

    Bitboard pieces_after_capture =
        (board.get_all_pieces_bb() & ~bitboard::create_bitboard(start_square) & ~captured_pawn_bb) |
        bitboard::create_bitboard(target_square);

    Bitboard slider_attackers =
        (magic::get_slider_attacks(king_square, piece::ROOK, pieces_after_capture) &
         (board.get_piece_bb(piece::ROOK, enemy_color) | enemy_queens)) |
        (magic::get_slider_attacks(king_square, piece::BISHOP, pieces_after_capture) &
         (board.get_piece_bb(piece::BISHOP, enemy_color) | enemy_queens));

    Bitboard remaining_checkers = checking_piece_bb & ~captured_pawn_bb &
                                  (board.get_piece_bb(piece::PAWN, enemy_color) |
                                   board.get_piece_bb(piece::KNIGHT, enemy_color));

    return slider_attackers == 0 && remaining_checkers == 0;
}
