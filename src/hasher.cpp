#include "hasher.hpp"

#include <array>
#include <cstdint>
#include <random>

#include "color.hpp"
#include "piece.hpp"
#include "position_info.hpp"
#include "square.hpp"

namespace hasher
{
namespace
{
struct Keys
{
    std::array<std::array<std::uint64_t, square::TOTAL_SQUARES>,
               piece::PIECE_TYPE_COUNT * color::COLORS>
        piece_keys;
    std::uint64_t to_move_key;
    std::uint64_t white_kingside_castle_key;
    std::uint64_t white_queenside_castle_key;
    std::uint64_t black_kingside_castle_key;
    std::uint64_t black_queenside_castle_key;
    std::array<std::uint64_t, square::FILES + 1> en_passant_keys;
};

Keys generate_keys()
{
    Keys keys{};
    std::mt19937_64 generator(std::random_device{}());
    std::uniform_int_distribution<std::uint64_t> distribution;

    for (auto& square_keys : keys.piece_keys)
    {
        for (std::uint64_t& key : square_keys)
        {
            key = distribution(generator);
        }
    }
    keys.to_move_key = distribution(generator);

    keys.white_kingside_castle_key = distribution(generator);
    keys.white_queenside_castle_key = distribution(generator);
    keys.black_kingside_castle_key = distribution(generator);
    keys.black_queenside_castle_key = distribution(generator);

    for (std::uint64_t& key : keys.en_passant_keys)
    {
        key = distribution(generator);
    }
    return keys;
}

const Keys keys = generate_keys();
} // namespace

void update_square(std::uint64_t& hash, Square square, Piece piece)
{
    const int piece_index = piece::get_piece_index(piece);
    if (piece_index != -1)
    {
        hash ^= keys.piece_keys[piece_index][square];
    }
}

void update_to_move(std::uint64_t& hash)
{
    hash ^= keys.to_move_key;
}

void update_castling_rights(std::uint64_t& hash, PositionInfo position_info)
{
    if (position_info::get_castling_right(position_info, color::WHITE, true))
    {
        hash ^= keys.white_kingside_castle_key;
    }

    if (position_info::get_castling_right(position_info, color::WHITE, false))
    {
        hash ^= keys.white_queenside_castle_key;
    }

    if (position_info::get_castling_right(position_info, color::BLACK, true))
    {
        hash ^= keys.black_kingside_castle_key;
    }

    if (position_info::get_castling_right(position_info, color::BLACK, false))
    {
        hash ^= keys.black_queenside_castle_key;
    }
}

void update_en_passant(std::uint64_t& hash, int file)
{
    hash ^= keys.en_passant_keys[file == -1 ? square::FILES : file];
}
} // namespace hasher
