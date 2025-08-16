# Developer Documentation

## Data Representation

### Square

Squares are indexed from 0 (a1) to 63 (h8):

```
56 57 58 59 60 61 62 63
48 49 50 51 52 53 54 55
40 41 42 43 44 45 46 47
32 33 34 35 36 37 38 39
24 25 26 27 28 29 30 31
16 17 18 19 20 21 22 23
 8  9 10 11 12 13 14 15
 0  1  2  3  4  5  6  7
```

### Piece

```
       Piece color
         (Black)
           ┌┴┐
 1   0   1  1
└────┬────┘
Piece type
  (Queen)
```

### Move

```
 1   0   1  0  0   0   1   0   0  0   0   0   1   1   0
└────┬────┘└──────────┬─────────┘└──────────┬──────────┘
   Flags        Target square          Start square
  (Castle)          (g1)                  (e1)
```

### Position info

```
                                         En passant file Captured piece
                                             (None)       (White pawn)
                                          ┌────┴────┐   ┌──────┴──────┐
 0   0   1   1   0   0   1  1   1   1   1  0   0   0  0  0   0   1   0  0
└────────────┬────────────┘└──────┬──────┘           └┬┘               └┬┘
       Fifty-move ply      Castling rights    En passant flag     Color to move
            (25)      (All castles are possible)    (no)             (White)
```

### Board

- Array of Pieces.
- Current Position Info + stack of previous Position Infos (allows unmaking moves).
- Current position hash + stack of previous hashes (avoids recomputing when unmaking moves) + hash map of seen hashes (to detect threefold repetitions).

### Bitboards

- 64-bit integers where each bit corresponds to a square on board.
- A bit set to 1 indicates that the corresponding square fulfills a certain condition.
- Example: `pinned_pieces_bb` has bits set for all squares with pinned pieces.
- Mostly used in Move Generation.

## Move Generation

1. Mark attacked squares, checks, pinned pieces, and blocking squares using bitboards.
2. Generate all possible moves for each piece (pseudo-legal moves).
3. Use the bitboards to filter pseudo-legal moves and find the legal moves.

## Evaluation

Evaluation of a position is expressed in centipawns (an evaluation of +100 means that White has an advantage of roughly one pawn).
In the analysis mode, the evaluation is displayed in whole pawns.
The evaluation is used to get the score of leaf nodes in the search.
The engine calculates the evaluation of a position based on two factors:

- Material : each piece type is assigned a base value (pawn: 100, knight: 300, bishop: 300, rook: 500, queen: 900).
- Piece-Square Tables : for each piece type, there is a table assigning an additional score based on the piece’s square (e.g., encouraging control of the center).

## Search

The engine explores possible moves and tries to find the best one.
The core searching algorithm is minimax with alpha-beta pruning.
To improve the pruning, moves are ordered based on certain factors (e.g., capturing moves have higher priority).
The engine uses iterative deepening: it searches first to depth 1, then from the beginning to depth 2, and so on.
After completing each depth, the core moves (from the root position) are ordered based on their scores from previous depth search.
Additionally, the engine uses quiescence search: when it reaches the maximum depth for the current search, it continues searching as long as it keeps playing "noisy" moves (captures and pawn promotions).
This reduces the chance of stopping calculation in a sharp position and mis-evaluating it (e.g., evaluating a position as winning after a queen exchange, even though the opponent can immediately recapture the queen).

## Hashing

The engine uses [Zobrist Hashing](https://www.chessprogramming.org/Zobrist_Hashing) to transform positions into numbers (hashes).
Position hashes are used to detect threefold repetitions.
The hashing works as follows:

- Random keys are generated for each piece-square combination, for when Black is to move, for each castling right and for each en passant file. These keys are generated once and used throughout the program.
- The hash of a position is calculated by XORing the keys of all the piece-square pairs in the position with the other keys representing the position state.
- To update the hash when the position changes, the engine XORs out the old key (XORing the same number twice cancels out) and XORs in the new key.
