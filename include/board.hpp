#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>

#include "bitboard.hpp"
#include "color.hpp"
#include "piece.hpp"
#include "position_info.hpp"
#include "square.hpp"
#include "types.hpp"

class Board
{
  public:
    static Square get_en_passant_capture_square(Square start_square, Square target_square);

    const std::array<Piece, square::TOTAL_SQUARES>& get_pieces() const;

    void load_position(const std::string& fen);

    bool is_occupied(Square square) const;

    Color get_piece_color(Square square) const;

    PieceType get_piece_type(Square square) const;

    void make_move(Move move);

    void unmake_move(Move move);

    Piece get_captured_piece(Move move) const;

    constexpr Bitboard get_color_bb(Color color) const
    {
        return color_bbs[color];
    }

    constexpr Bitboard get_piece_bb(PieceType piece_type, Color color) const
    {
        return piece_type_bbs[piece_type] & color_bbs[color];
    }

    constexpr Bitboard get_all_pieces_bb() const
    {
        return color_bbs[color::WHITE] | color_bbs[color::BLACK];
    }

    constexpr Square get_king_square(Color color) const
    {
        return bitboard::get_square(get_piece_bb(piece::KING, color));
    }

    constexpr Color get_to_move() const
    {
        return position_info::get_to_move(position_info);
    }

    constexpr int get_fifty_move_ply() const
    {
        return position_info::get_fifty_move_ply(position_info);
    }

    constexpr bool get_castling_right(Color color, bool short_castle) const
    {
        return position_info::get_castling_right(position_info, color, short_castle);
    }

    constexpr Square get_en_passant_square() const
    {
        int en_passant_file = position_info::get_en_passant_file(position_info);
        if (en_passant_file == -1)
        {
            return -1;
        }
        int rank = get_to_move() == color::WHITE ? 5 : 2;
        return square::create_square(en_passant_file, rank);
    }

    constexpr int get_ply() const
    {
        return ply;
    }

    bool is_repetition(int root_ply) const;

  private:
    static constexpr std::array<Square, color::COLORS> KING_START_SQUARE = {square::e1, square::e8};
    static constexpr std::array<Square, color::COLORS> QUEENSIDE_ROOK_START_SQUARE = {square::a1,
                                                                                      square::a8};
    static constexpr std::array<Square, color::COLORS> KINGSIDE_ROOK_START_SQUARE = {square::h1,
                                                                                     square::h8};

    std::array<Piece, square::TOTAL_SQUARES> pieces{};
    std::array<Bitboard, piece::PIECE_TYPE_COUNT + 1> piece_type_bbs{};
    std::array<Bitboard, color::COLORS> color_bbs{};
    PositionInfo position_info = 0;
    static constexpr int MAX_PLIES = 1024;
    std::array<PositionInfo, MAX_PLIES> previous_positions{};
    std::array<uint64_t, MAX_PLIES> previous_hashes{};
    int ply = 0;
    uint64_t hash = 0;

    void place_piece(Square square, Piece piece);

    void remove_piece(Square square);

    void move_piece(Square start_square, Square target_square);

    void reset();
};

#endif
