#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <stack>
#include <string>
#include <unordered_map>

#include "piece.hpp"
#include "square.hpp"
#include "types.hpp"

class Board
{
  public:
    static Square get_en_passant_square(int en_passant_file, Color to_move);

    static Square get_en_passant_capture_square(Square start_square, Square target_square);

    const std::array<Piece, square::TOTAL_SQUARES>& get_pieces() const;
    PositionInfo get_position_info() const;

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

    bool is_threefold_repetition() const;

  private:
    static constexpr std::array<Square, piece::COLORS> KING_START_SQUARE = {square::e1, square::e8};
    static constexpr std::array<Square, piece::COLORS> QUEENSIDE_ROOK_START_SQUARE = {square::a1, square::a8};
    static constexpr std::array<Square, piece::COLORS> KINGSIDE_ROOK_START_SQUARE = {square::h1, square::h8};

    std::array<Piece, square::TOTAL_SQUARES> pieces{};
    std::array<Bitboard, piece::PIECE_TYPE_COUNT + 1> piece_type_bbs{};
    std::array<Bitboard, piece::COLORS> color_bbs{};
    PositionInfo position_info = 0;
    std::stack<PositionInfo> previous_positions;
    std::stack<uint64_t> previous_hashes;
    std::unordered_map<uint64_t, int> hash_count;
    uint64_t hash = 0;

    void place_piece(Square square, Piece piece);

    void remove_piece(Square square);

    void move_piece(Square start_square, Square target_square);

    void reset();
};

#endif
