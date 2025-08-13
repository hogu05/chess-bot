# Chess Bot

Implementation of chess engine with a terminal UI.

## Build

Create a build directory and move into it:

```bash
mkdir build
cd build
```

Compile the code:

```bash
cmake .. && make
```

## Rules

Engine follows the [FIDE chess rules](https://www.fide.com/FIDE/handbook/LawsOfChess.pdf).
It ignores the fifty move rule (9.3 in the handbook).

## Usage

Inside the `build` directory, run the program with:

```bash
./chess-bot
```

## Terminal UI

Interaction with the engine is done through a read–eval–print loop (REPL) interface.

```

  +-----------------+
8 | r n b q k b n r |
7 | p p p p p p p p |
6 | . . . . . . . . |
5 | . . . . . . . . |
4 | . . . . . . . . |
3 | . . . . . . . . |
2 | P P P P P P P P |
1 | R N B Q K B N R |
  +-----------------+
W   a b c d e f g h   I

Welcome to the Chess Bot, type 'help' for a list of commands
>

```

The interface is divided into several parts:

- **Chessboard** (top): Displays the current position from White's perspective.
- **Turn indicator** (bottom-left of chessboard): Indicates whose turn it is (W: White, B: Black).
- **Mode indicator** (bottom-right of chessboard): Indicates which mode is currently active.
- **Info line** (below the board): Displays messages such as command feedback, analysis results and game updates.
- **Command line** (below info line): Line for entering commands.

## Commands

The engine has multiple modes, each with its set of commands.

### Common commands

- `help` : Show commands for the current mode.
- `quit` : Exit the program.

### Idle mode

Default mode when no analysis or game is in progress.

- `load <FEN>` : Load a position from FEN (default: starting position).
- `move <notation>` : Make a move by entering the starting square followed by the ending square (e.g., e2e4).
- `go` : Start the engine analysis.
- `play [time]` : Start playing against the engine from the current position (the engine moves first), optionally specifying the bot's thinking time in milliseconds (default: 3000 ms).
- `display <mode>` : Change the board display mode (letters or unicode).

### Analysis mode

Engine analyses the current position and displays the results on the info line.

- `stop` : Stop the engine analysis.

### Play mode

The user plays against the engine. After the user makes their move, they wait for the engine to respond.

- `<notation>` : Play a move by entering the starting square followed by the ending square (e.g., e2e4).
- `stop` : Stop the game.

## Perft (performance test)

Inside the `build` directory, run the perft with:

```bash
./perft
```

Perft tests move generation by generating all possible positions from a given position up to a specified depth and comparing the result with the known correct number of positions.
It also reports how fast the move generation is in kN/s (kiloNodes per second):

```
Test    Nodes       Expected    Time(ms)  kN/s        Result
0       119060324   119060324   7211      16510       ✓
1       4085603     4085603     207       19737       ✓
2       11030083    11030083    644       17127       ✓
3       15833292    15833292    805       19668       ✓
4       89941194    89941194    4366      20600       ✓
------------------------------------------------------------
All tests passed!
Total nodes: 239950496 | Total time: 13233ms | Average speed: 18132kN/s
```
