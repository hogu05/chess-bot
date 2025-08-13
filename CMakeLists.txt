cmake_minimum_required(VERSION 3.28)
project(chess-bot)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

add_library(chess_core
    src/bot.cpp
    src/board.cpp
    src/piece.cpp
    src/move_generator.cpp
    src/move.cpp
    src/precomputations.cpp
    src/directions.cpp
    src/bitboard.cpp
    src/position_info.cpp
    src/evaluator.cpp
    src/piece_square_tables.cpp
    src/hasher.cpp
)

target_include_directories(chess_core PUBLIC include)

add_executable(chess-bot
    src/main.cpp
    src/repl.cpp
)
target_link_libraries(chess-bot PRIVATE chess_core)

add_executable(tests
    tests/perft.cpp
)
target_link_libraries(tests PRIVATE chess_core)

