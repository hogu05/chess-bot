#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "types.hpp"

class TranspositionTable
{
  public:
    enum class Bound : std::uint8_t
    {
        EXACT,
        LOWER,
        UPPER
    };

    struct Entry
    {
        std::uint64_t hash;
        int depth;
        int score;
        Bound bound;
        Move best_move;
    };

    const Entry* probe(std::uint64_t hash) const;

    void store(std::uint64_t hash, int depth, int score, Bound bound, Move best_move);

  private:
    static constexpr std::size_t SIZE = 1 << 20;
    static constexpr std::size_t MASK = SIZE - 1;

    std::vector<Entry> entries = std::vector<Entry>(SIZE);
};

#endif
