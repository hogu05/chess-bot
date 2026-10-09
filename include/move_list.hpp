#ifndef MOVE_LIST_H
#define MOVE_LIST_H

#include <array>

#include "types.hpp"

class MoveList
{
  public:
    static constexpr int MAX_MOVES = 256;

    constexpr void push_back(Move move)
    {
        moves[count++] = move;
    }

    constexpr int size() const
    {
        return count;
    }

    constexpr bool empty() const
    {
        return count == 0;
    }

    constexpr Move* begin()
    {
        return moves.data();
    }

    constexpr Move* end()
    {
        return moves.data() + count;
    }

    constexpr Move& operator[](int index)
    {
        return moves[index];
    }

  private:
    std::array<Move, MAX_MOVES> moves{};
    int count = 0;
};

#endif
