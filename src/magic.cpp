#include "magic.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include "directions.hpp"
#include "piece.hpp"
#include "precomputations.hpp"
#include "square.hpp"

namespace magic
{
namespace
{
struct Magic
{
    Bitboard mask;
    Bitboard number;
    int bits;
    int offset;
};

using Magics = std::array<Magic, square::TOTAL_SQUARES>;

constexpr std::array<Bitboard, square::TOTAL_SQUARES> ROOK_MAGIC_NUMBERS = {
    0x1080004008801020ULL, 0x0840092002C03000ULL, 0x1900200010400900ULL, 0x0880100008000480ULL,
    0x4200100420080200ULL, 0x8100020100080400ULL, 0x0200040110886200ULL, 0x0200008040220411ULL,
    0x0404800084400220ULL, 0x0000401000402000ULL, 0x0086001081220440ULL, 0x0408800800100280ULL,
    0x000A001201040820ULL, 0x8848800200840080ULL, 0x4001000100040200ULL, 0x0442000102105084ULL,
    0x9080010020804100ULL, 0x0040404000201009ULL, 0x0000808010002009ULL, 0x2200090021D00100ULL,
    0x0008008008040080ULL, 0x0004004002010040ULL, 0x0011040008015042ULL, 0x00000A0001768104ULL,
    0x0000800080204009ULL, 0x2010004140002001ULL, 0x9800200280100080ULL, 0x1000100080080080ULL,
    0x0442000A00049020ULL, 0x2100040080020080ULL, 0x0800120400900148ULL, 0x0010040A00128541ULL,
    0x2800804000800030ULL, 0x1010002000400041ULL, 0x4000200011004100ULL, 0x0610008410800800ULL,
    0x0400802402800800ULL, 0xC100020080800400ULL, 0x0002000802000401ULL, 0x0182085882000401ULL,
    0x0220204000808000ULL, 0x2860100040024022ULL, 0x0001002004110040ULL, 0x99101042000A0020ULL,
    0x0004080004008080ULL, 0x0010040002008080ULL, 0x2012004881020004ULL, 0x8300842444820011ULL,
    0x0088403882010200ULL, 0x0820400080210100ULL, 0x0110910040A00300ULL, 0x0801100280080480ULL,
    0x0242009008200600ULL, 0x1002000489500200ULL, 0x0040800200010080ULL, 0x0091800041000080ULL,
    0x0000209300488001ULL, 0x04C1002414824001ULL, 0x020020000B001041ULL, 0x7000100004200901ULL,
    0x8002002004100802ULL, 0x30010002084C0007ULL, 0x0888221800813004ULL, 0x4000002840840112ULL,
};

constexpr std::array<Bitboard, square::TOTAL_SQUARES> BISHOP_MAGIC_NUMBERS = {
    0xA010041108003100ULL, 0x006082020A002900ULL, 0x6810010619200000ULL, 0x08281A0520000408ULL,
    0x0001104001000400ULL, 0x0018901008048400ULL, 0x00040A0210245280ULL, 0x000200210808A402ULL,
    0x9140048410821200ULL, 0x0800091010820041ULL, 0x20504804832202C0ULL, 0x0100091401081000ULL,
    0x8021011140000012ULL, 0x0810020804450400ULL, 0x208B0542109008A2ULL, 0x0080084A08040204ULL,
    0x0040E2A80811244CULL, 0x2505022008008108ULL, 0x0430220100420040ULL, 0x010A040420220040ULL,
    0x1105000290400000ULL, 0x0093001200822120ULL, 0x4000A62048043004ULL, 0x280120048A015004ULL,
    0x006090002A020814ULL, 0x44042000240800D0ULL, 0x01102800040A4400ULL, 0x1004080080220040ULL,
    0x0001001011004024ULL, 0x0010044000805040ULL, 0x0914041200820100ULL, 0x0004821012821480ULL,
    0x0024040500C05021ULL, 0x0088611002080200ULL, 0x0116080A00040020ULL, 0x4000020080080080ULL,
    0x2450450140840040ULL, 0x0000880201484100ULL, 0x0222020404020092ULL, 0x8081110600002E00ULL,
    0x2842101105000801ULL, 0x1100809008001025ULL, 0x00020202221C0400ULL, 0x0422014022009020ULL,
    0x0210046102100C00ULL, 0xC004008082029102ULL, 0x00AA461801101200ULL, 0x0404080080201108ULL,
    0x020542108C205002ULL, 0x0410544804100100ULL, 0x0040910841100000ULL, 0x0400200042021100ULL,
    0x00004204850400C0ULL, 0x0200100410A42102ULL, 0x1040020801210102ULL, 0x0805040410420000ULL,
    0x2884804130100200ULL, 0x800C262201242000ULL, 0x1058000194108800ULL, 0x0014221054420204ULL,
    0x0104000012A02200ULL, 0x0200881003300100ULL, 0x0140400202840100ULL, 0x0402020801010201ULL,
};

Bitboard compute_attacks(Square square, Bitboard pieces, bool is_rook)
{
    Bitboard attacks = 0;

    int start_index = is_rook ? 0 : directions::LAST_ORTHOGONAL_DIRECTION_INDEX + 1;
    int end_index = is_rook ? directions::LAST_ORTHOGONAL_DIRECTION_INDEX
                            : directions::sliding_directions.size() - 1;
    for (int direction_index = start_index; direction_index <= end_index; direction_index++)
    {
        Direction direction = directions::sliding_directions[direction_index];
        for (int i = 1; i <= precomputations::get_squares_to_edge(square, direction); i++)
        {
            Square target_square = square + (direction * i);
            bitboard::set_square(attacks, target_square);
            if (bitboard::is_set(pieces, target_square))
            {
                break;
            }
        }
    }
    return attacks;
}

Bitboard compute_mask(Square square, bool is_rook)
{
    Bitboard mask = 0;

    int start_index = is_rook ? 0 : directions::LAST_ORTHOGONAL_DIRECTION_INDEX + 1;
    int end_index = is_rook ? directions::LAST_ORTHOGONAL_DIRECTION_INDEX
                            : directions::sliding_directions.size() - 1;
    for (int direction_index = start_index; direction_index <= end_index; direction_index++)
    {
        Direction direction = directions::sliding_directions[direction_index];
        for (int i = 1; i <= precomputations::get_squares_to_edge(square, direction) - 1; i++)
        {
            bitboard::set_square(mask, square + (direction * i));
        }
    }
    return mask;
}

Magics compute_magics(const std::array<Bitboard, square::TOTAL_SQUARES>& magic_numbers,
                      bool is_rook)
{
    Magics magics{};
    int offset = 0;
    for (Square square = 0; square < square::TOTAL_SQUARES; ++square)
    {
        Bitboard mask = compute_mask(square, is_rook);
        int bits = std::popcount(mask);
        magics[square] = Magic(mask, magic_numbers[square], bits, offset);
        offset += 1 << bits;
    }
    return magics;
}

std::vector<Bitboard> compute_attack_table(const Magics& magics, bool is_rook)
{
    std::vector<Bitboard> attack_table;
    for (Square square = 0; square < square::TOTAL_SQUARES; ++square)
    {
        const Magic& magic = magics[square];
        attack_table.resize(attack_table.size() + (1ULL << magic.bits));
        Bitboard blockers = 0;
        for (std::size_t pattern = 0; pattern < (1ULL << magic.bits); ++pattern)
        {
            std::size_t index = (blockers * magic.number) >> (64 - magic.bits);
            Bitboard blocker_attacks = compute_attacks(square, blockers, is_rook);
            Bitboard& slot = attack_table[magic.offset + index];
            if (slot != 0 && slot != blocker_attacks)
            {
                std::abort();
            }
            slot = blocker_attacks;
            blockers = (blockers - magic.mask) & magic.mask;
        }
    }
    return attack_table;
}

const Magics rook_magics = compute_magics(ROOK_MAGIC_NUMBERS, true);

const Magics bishop_magics = compute_magics(BISHOP_MAGIC_NUMBERS, false);

const std::vector<Bitboard> rook_attacks = compute_attack_table(rook_magics, true);

const std::vector<Bitboard> bishop_attacks = compute_attack_table(bishop_magics, false);

Bitboard lookup(const Magic& magic, const std::vector<Bitboard>& attack_table, Bitboard pieces)
{
    std::size_t index = ((pieces & magic.mask) * magic.number) >> (64 - magic.bits);
    return attack_table[magic.offset + index];
}
} // namespace

Bitboard get_slider_attacks(Square square, PieceType piece_type, Bitboard pieces)
{
    switch (piece_type)
    {
    case piece::BISHOP:
        return lookup(bishop_magics[square], bishop_attacks, pieces);
    case piece::ROOK:
        return lookup(rook_magics[square], rook_attacks, pieces);
    default:
        return lookup(bishop_magics[square], bishop_attacks, pieces) |
               lookup(rook_magics[square], rook_attacks, pieces);
    }
}
} // namespace magic
