#include "hasher.hpp"

#include <random>

#include "board.hpp"
#include "piece.hpp"
#include "position_info.hpp"

namespace Hasher
{
std::array<std::array<uint64_t, Board::TOTAL_SQUARES>, Piece::PIECE_TYPE_COUNT * Board::COLORS>
    piece_keys;
uint64_t to_move_key;
uint64_t white_short_castle_key;
uint64_t white_long_castle_key;
uint64_t black_short_castle_key;
uint64_t black_long_castle_key;
std::array<uint64_t, Board::FILES + 1> en_passant_keys;

void init_hasher()
{
    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dis;

    for (int piece_index = 0; piece_index < Piece::PIECE_TYPE_COUNT * 2; piece_index++)
    {
        for (Square_t square = 0; square < Board::TOTAL_SQUARES; square++)
        {
            piece_keys[piece_index][square] = dis(gen);
        }
    }
    to_move_key = dis(gen);

    white_short_castle_key = dis(gen);
    white_long_castle_key = dis(gen);
    black_short_castle_key = dis(gen);
    black_long_castle_key = dis(gen);

    for (int i = 0; i < Board::FILES + 1; i++)
    {
        en_passant_keys[i] = dis(gen);
    }
}

void update_square(uint64_t& hash, Square_t square, Piece_t piece)
{
    int piece_index = Piece::get_piece_index(piece);
    if (piece_index != -1)
    {
        hash ^= piece_keys[piece_index][square];
    }
}
void update_to_move(uint64_t& hash, Color_t color)
{
    hash ^= to_move_key * color;
}

void update_castling_rights(uint64_t& hash, PositionInfo_t position_info)
{
    if (PositionInfo::get_castling_right(position_info, Piece::WHITE, true))
    {
        hash ^= white_short_castle_key;
    }

    if (PositionInfo::get_castling_right(position_info, Piece::WHITE, false))
    {
        hash ^= white_long_castle_key;
    }

    if (PositionInfo::get_castling_right(position_info, Piece::BLACK, true))
    {
        hash ^= black_short_castle_key;
    }

    if (PositionInfo::get_castling_right(position_info, Piece::BLACK, false))
    {
        hash ^= black_long_castle_key;
    }
}

void update_en_passant(uint64_t& hash, int file)
{
    file = file == -1 ? 8 : file;
    hash ^= en_passant_keys[file];
}
} // namespace Hasher
