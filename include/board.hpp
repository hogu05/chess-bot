#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <cstdint>
#include <string>

#include "bitboard.hpp"
#include "color.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "position_info.hpp"
#include "square.hpp"
#include "types.hpp"

class Board
{
  public:
    static constexpr Square get_en_passant_capture_square(Square start_square, Square target_square)
    {
        return square::create_square(square::get_file(target_square),
                                     square::get_rank(start_square));
    }

    void load_position(const std::string& fen);

    void make_move(Move move);

    void unmake_move(Move move);

    bool is_repetition(int root_ply) const;

    constexpr const std::array<Piece, square::TOTAL_SQUARES>& get_squares() const
    {
        return squares;
    }

    constexpr PieceType get_piece_type(Square square) const
    {
        return piece::get_piece_type(squares[square]);
    }

    constexpr Piece get_captured_piece(Move move) const
    {
        const Square target_square = move::get_target_square(move);
        if (move::get_flag(move) == move::EN_PASSANT_FLAG)
        {
            return squares[get_en_passant_capture_square(move::get_start_square(move),
                                                         target_square)];
        }
        return squares[target_square];
    }

    constexpr Bitboard get_pieces(Color color) const
    {
        return pieces_by_color[color];
    }

    constexpr Bitboard get_pieces(PieceType piece_type, Color color) const
    {
        return pieces_by_type[piece_type] & pieces_by_color[color];
    }

    constexpr Bitboard get_all_pieces() const
    {
        return pieces_by_color[color::WHITE] | pieces_by_color[color::BLACK];
    }

    constexpr Square get_king_square(Color color) const
    {
        return bitboard::get_square(get_pieces(piece::KING, color));
    }

    constexpr Color get_to_move() const
    {
        return position_info::get_to_move(position_info);
    }

    constexpr int get_fifty_move_ply() const
    {
        return position_info::get_fifty_move_ply(position_info);
    }

    constexpr bool get_castling_right(Color color, bool kingside) const
    {
        return position_info::get_castling_right(position_info, color, kingside);
    }

    constexpr Square get_en_passant_square() const
    {
        const int en_passant_file = position_info::get_en_passant_file(position_info);
        if (en_passant_file == -1)
        {
            return -1;
        }
        return square::create_square(en_passant_file, get_to_move() == color::WHITE ? 5 : 2);
    }

    constexpr int get_ply() const
    {
        return ply;
    }

    constexpr std::uint64_t get_hash() const
    {
        return hash;
    }

  private:
    static constexpr std::array<Square, color::COLORS> KING_START_SQUARE = {square::e1, square::e8};
    static constexpr std::array<Square, color::COLORS> QUEENSIDE_ROOK_START_SQUARE = {square::a1,
                                                                                      square::a8};
    static constexpr std::array<Square, color::COLORS> KINGSIDE_ROOK_START_SQUARE = {square::h1,
                                                                                     square::h8};
    static constexpr int MAX_PLIES = 1024;

    std::array<Piece, square::TOTAL_SQUARES> squares{};
    std::array<Bitboard, piece::PIECE_TYPE_COUNT + 1> pieces_by_type{};
    std::array<Bitboard, color::COLORS> pieces_by_color{};
    PositionInfo position_info = 0;
    std::array<PositionInfo, MAX_PLIES> previous_positions{};
    std::array<std::uint64_t, MAX_PLIES> previous_hashes{};
    int ply = 0;
    std::uint64_t hash = 0;

    void place_piece(Square square, Piece piece);

    void remove_piece(Square square);

    void move_piece(Square start_square, Square target_square);

    void reset();
};

#endif
