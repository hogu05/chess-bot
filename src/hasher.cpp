#include "hasher.hpp"

#include <array>
#include <random>

#include "piece.hpp"
#include "position_info.hpp"
#include "square.hpp"

namespace hasher
{
namespace
{
struct Keys
{
    std::array<std::array<uint64_t, square::TOTAL_SQUARES>, piece::PIECE_TYPE_COUNT * piece::COLORS>
        piece_keys;
    uint64_t to_move_key;
    uint64_t white_short_castle_key;
    uint64_t white_long_castle_key;
    uint64_t black_short_castle_key;
    uint64_t black_long_castle_key;
    std::array<uint64_t, square::FILES + 1> en_passant_keys;
};

Keys generate_keys()
{
    Keys keys{};
    std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dis;

    for (int piece_index = 0; piece_index < piece::PIECE_TYPE_COUNT * 2; piece_index++)
    {
        for (Square square = 0; square < square::TOTAL_SQUARES; square++)
        {
            keys.piece_keys[piece_index][square] = dis(gen);
        }
    }
    keys.to_move_key = dis(gen);

    keys.white_short_castle_key = dis(gen);
    keys.white_long_castle_key = dis(gen);
    keys.black_short_castle_key = dis(gen);
    keys.black_long_castle_key = dis(gen);

    for (int i = 0; i < square::FILES + 1; i++)
    {
        keys.en_passant_keys[i] = dis(gen);
    }
    return keys;
}

const Keys keys = generate_keys();
} // namespace

void update_square(uint64_t& hash, Square square, Piece piece)
{
    int piece_index = piece::get_piece_index(piece);
    if (piece_index != -1)
    {
        hash ^= keys.piece_keys[piece_index][square];
    }
}
void update_to_move(uint64_t& hash, Color color)
{
    hash ^= keys.to_move_key * color;
}

void update_castling_rights(uint64_t& hash, PositionInfo position_info)
{
    if (position_info::get_castling_right(position_info, piece::WHITE, true))
    {
        hash ^= keys.white_short_castle_key;
    }

    if (position_info::get_castling_right(position_info, piece::WHITE, false))
    {
        hash ^= keys.white_long_castle_key;
    }

    if (position_info::get_castling_right(position_info, piece::BLACK, true))
    {
        hash ^= keys.black_short_castle_key;
    }

    if (position_info::get_castling_right(position_info, piece::BLACK, false))
    {
        hash ^= keys.black_long_castle_key;
    }
}

void update_en_passant(uint64_t& hash, int file)
{
    file = file == -1 ? 8 : file;
    hash ^= keys.en_passant_keys[file];
}
} // namespace hasher
