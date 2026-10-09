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

MoveList MoveGenerator::get_moves()
{
    update_checks_and_pins();
    MoveList moves;
    const Color color = board.get_to_move();

    add_king_moves(moves, get_friendly_king_square(), color);
    if (is_double_check)
    {
        return moves;
    }

    Bitboard pawns = board.get_pieces(piece::PAWN, color);
    while (pawns != 0)
    {
        const Square square = bitboard::pop_square(pawns);
        update_pinned_piece_possible_squares(square);
        add_pawn_moves(moves, square, color);
    }

    Bitboard knights = board.get_pieces(piece::KNIGHT, color);
    while (knights != 0)
    {
        const Square square = bitboard::pop_square(knights);
        update_pinned_piece_possible_squares(square);
        add_knight_moves(moves, square);
    }

    for (const PieceType piece_type : {piece::BISHOP, piece::ROOK, piece::QUEEN})
    {
        Bitboard sliders = board.get_pieces(piece_type, color);
        while (sliders != 0)
        {
            const Square square = bitboard::pop_square(sliders);
            update_pinned_piece_possible_squares(square);
            add_sliding_piece_moves(moves, square, piece_type);
        }
    }
    return moves;
}

bool MoveGenerator::is_check() const
{
    return checkers != 0;
}

std::uint64_t MoveGenerator::perft(int depth)
{
    MoveList moves = get_moves();
    if (depth == 1)
    {
        return moves.size();
    }

    std::uint64_t nodes = 0;
    for (const Move move : moves)
    {
        board.make_move(move);
        nodes += perft(depth - 1);
        board.unmake_move(move);
    }
    return nodes;
}

Bitboard MoveGenerator::get_friendly_pieces() const
{
    return board.get_pieces(board.get_to_move());
}

Bitboard MoveGenerator::get_enemy_pieces() const
{
    return board.get_pieces(color::get_other_color(board.get_to_move()));
}

Square MoveGenerator::get_friendly_king_square() const
{
    return board.get_king_square(board.get_to_move());
}

void MoveGenerator::update_checks_and_pins()
{
    update_attacks();

    pinned_pieces = 0;
    bitboard::set_all(blocking_squares);
    if (is_double_check)
    {
        return;
    }

    pinned_pieces = get_pinned_pieces();
    if (checkers != 0)
    {
        blocking_squares =
            precomputations::between[get_friendly_king_square()][bitboard::get_square(checkers)];
    }
}

void MoveGenerator::update_attacks()
{
    const Color friendly_color = board.get_to_move();
    const Color enemy_color = color::get_other_color(friendly_color);
    const Square king_square = get_friendly_king_square();

    Bitboard enemy_pawns = board.get_pieces(piece::PAWN, enemy_color);
    Bitboard enemy_knights = board.get_pieces(piece::KNIGHT, enemy_color);

    checkers = (precomputations::pawn_attacks[friendly_color][king_square] & enemy_pawns) |
               (precomputations::knight_moves[king_square] & enemy_knights);
    attacked_squares = precomputations::king_moves[board.get_king_square(enemy_color)];

    while (enemy_pawns != 0)
    {
        attacked_squares |=
            precomputations::pawn_attacks[enemy_color][bitboard::pop_square(enemy_pawns)];
    }

    while (enemy_knights != 0)
    {
        attacked_squares |= precomputations::knight_moves[bitboard::pop_square(enemy_knights)];
    }

    for (const PieceType piece_type : {piece::BISHOP, piece::ROOK, piece::QUEEN})
    {
        Bitboard sliders = board.get_pieces(piece_type, enemy_color);
        while (sliders != 0)
        {
            const Square slider_square = bitboard::pop_square(sliders);
            const Bitboard slider_attacks = magic::get_slider_attacks(
                slider_square, piece_type,
                board.get_all_pieces() & ~bitboard::create_bitboard(king_square));
            if (bitboard::is_set(slider_attacks, king_square))
            {
                bitboard::set_square(checkers, slider_square);
            }
            attacked_squares |= slider_attacks;
        }
    }

    is_double_check = std::popcount(checkers) > 1;
}

Bitboard MoveGenerator::get_pinned_pieces() const
{
    const Square king_square = get_friendly_king_square();
    const Color enemy_color = color::get_other_color(board.get_to_move());
    const Bitboard enemy_queens = board.get_pieces(piece::QUEEN, enemy_color);

    Bitboard snipers = (magic::get_slider_attacks(king_square, piece::ROOK, get_enemy_pieces()) &
                        (board.get_pieces(piece::ROOK, enemy_color) | enemy_queens)) |
                       (magic::get_slider_attacks(king_square, piece::BISHOP, get_enemy_pieces()) &
                        (board.get_pieces(piece::BISHOP, enemy_color) | enemy_queens));

    Bitboard pinned = 0;
    while (snipers != 0)
    {
        const Bitboard blockers =
            precomputations::between[king_square][bitboard::pop_square(snipers)] &
            board.get_all_pieces();
        if (std::popcount(blockers) == 1)
        {
            pinned |= blockers;
        }
    }
    return pinned;
}

void MoveGenerator::update_pinned_piece_possible_squares(Square square)
{
    bitboard::set_all(pinned_piece_possible_squares);
    if (bitboard::is_set(pinned_pieces, square))
    {
        pinned_piece_possible_squares = precomputations::line[get_friendly_king_square()][square];
    }
}

Bitboard MoveGenerator::get_legal_squares(Bitboard squares) const
{
    return squares & (blocking_squares | checkers) & pinned_piece_possible_squares;
}

void MoveGenerator::add_king_moves(MoveList& moves, Square square, Color color) const
{
    move::add_moves_from_bitboard(moves, square,
                                  precomputations::king_moves[square] & ~get_friendly_pieces() &
                                      ~attacked_squares);

    if (checkers != 0)
    {
        return;
    }

    const Bitboard all_pieces = board.get_all_pieces();
    if (board.get_castling_right(color, true) && bitboard::is_clear(all_pieces, square + 1) &&
        bitboard::is_clear(all_pieces, square + 2) &&
        bitboard::is_clear(attacked_squares, square + 1) &&
        bitboard::is_clear(attacked_squares, square + 2))
    {
        moves.push_back(move::create_move(square, square + 2, move::CASTLE_FLAG));
    }

    if (board.get_castling_right(color, false) && bitboard::is_clear(all_pieces, square - 1) &&
        bitboard::is_clear(all_pieces, square - 2) && bitboard::is_clear(all_pieces, square - 3) &&
        bitboard::is_clear(attacked_squares, square - 1) &&
        bitboard::is_clear(attacked_squares, square - 2))
    {
        moves.push_back(move::create_move(square, square - 2, move::CASTLE_FLAG));
    }
}

void MoveGenerator::add_pawn_moves(MoveList& moves, Square square, Color color) const
{
    const Bitboard all_pieces = board.get_all_pieces();
    const Square en_passant_square = board.get_en_passant_square();
    const Direction direction = directions::pawn_directions[color];
    const Square push_square = square + direction;
    const Square double_push_square = push_square + direction;
    const bool is_promotion = piece::can_pawn_promote(square, color);

    if (bitboard::is_clear(all_pieces, push_square))
    {
        if (get_legal_squares(bitboard::create_bitboard(push_square)) != 0)
        {
            add_pawn_move(moves, square, push_square, move::NO_FLAG, is_promotion);
        }
        if (piece::can_pawn_move_two_spaces(square, color) &&
            bitboard::is_clear(all_pieces, double_push_square) &&
            get_legal_squares(bitboard::create_bitboard(double_push_square)) != 0)
        {
            moves.push_back(
                move::create_move(square, double_push_square, move::TWO_SPACE_PAWN_MOVE_FLAG));
        }
    }

    Bitboard captures =
        get_legal_squares(precomputations::pawn_attacks[color][square] & get_enemy_pieces());
    while (captures != 0)
    {
        add_pawn_move(moves, square, bitboard::pop_square(captures), move::NO_FLAG, is_promotion);
    }

    if (en_passant_square != -1 &&
        bitboard::is_set(precomputations::pawn_attacks[color][square], en_passant_square) &&
        is_en_passant_legal(square, en_passant_square))
    {
        moves.push_back(move::create_move(square, en_passant_square, move::EN_PASSANT_FLAG));
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
    move::add_moves_from_bitboard(
        moves, square,
        get_legal_squares(precomputations::knight_moves[square] & ~get_friendly_pieces()));
}

void MoveGenerator::add_sliding_piece_moves(MoveList& moves, Square square,
                                            PieceType piece_type) const
{
    move::add_moves_from_bitboard(
        moves, square,
        get_legal_squares(magic::get_slider_attacks(square, piece_type, board.get_all_pieces()) &
                          ~get_friendly_pieces()));
}

bool MoveGenerator::is_en_passant_legal(Square start_square, Square target_square) const
{
    const Bitboard captured_pawn = bitboard::create_bitboard(
        Board::get_en_passant_capture_square(start_square, target_square));
    const Square king_square = get_friendly_king_square();
    const Color enemy_color = color::get_other_color(board.get_to_move());
    const Bitboard enemy_queens = board.get_pieces(piece::QUEEN, enemy_color);
    const Bitboard pieces_after_capture =
        (board.get_all_pieces() & ~bitboard::create_bitboard(start_square) & ~captured_pawn) |
        bitboard::create_bitboard(target_square);

    if ((magic::get_slider_attacks(king_square, piece::ROOK, pieces_after_capture) &
         (board.get_pieces(piece::ROOK, enemy_color) | enemy_queens)) != 0 ||
        (magic::get_slider_attacks(king_square, piece::BISHOP, pieces_after_capture) &
         (board.get_pieces(piece::BISHOP, enemy_color) | enemy_queens)) != 0)
    {
        return false;
    }

    return (checkers & ~captured_pawn &
            (board.get_pieces(piece::PAWN, enemy_color) |
             board.get_pieces(piece::KNIGHT, enemy_color))) == 0;
}
