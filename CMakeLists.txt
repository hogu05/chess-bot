cmake_minimum_required(VERSION 3.28)
project(chess-bot)

set(CMAKE_CXX_STANDARD 26)

include_directories(include)

set(CHESS_SOURCES
    src/board.cpp
    src/piece.cpp
    src/move_generator.cpp
    src/move.cpp
    src/precomputations.cpp
    src/directions.cpp
    src/bitboard.cpp
    src/position_info.cpp
    src/evaluator.cpp
)

add_executable(chess-bot
    src/main.cpp
    ${CHESS_SOURCES}
)

add_executable(tests
    tests/perft.cpp
    ${CHESS_SOURCES}
)
