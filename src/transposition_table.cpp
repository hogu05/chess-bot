#include "transposition_table.hpp"

#include <cstdint>

#include "types.hpp"

const TranspositionTable::Entry* TranspositionTable::probe(std::uint64_t hash) const
{
    const Entry& entry = entries[hash & MASK];
    return entry.hash == hash ? &entry : nullptr;
}

void TranspositionTable::store(std::uint64_t hash, int depth, int score, Bound bound,
                               Move best_move)
{
    entries[hash & MASK] = Entry(hash, depth, score, bound, best_move);
}
