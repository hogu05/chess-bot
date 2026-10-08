cmake_minimum_required(VERSION 3.28)
project(chess-bot)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_library(chess_core
    src/bot.cpp
    src/board.cpp
    src/piece.cpp
    src/move_generator.cpp
    src/move.cpp
    src/directions.cpp
    src/position_info.cpp
    src/evaluator.cpp
    src/piece_square_tables.cpp
    src/hasher.cpp
    src/notation.cpp
)

target_include_directories(chess_core PUBLIC include)

add_executable(perft
    tests/perft.cpp
)
target_link_libraries(perft PRIVATE chess_core)

add_executable(chess-bot-uci
    src/uci.cpp
)
target_link_libraries(chess-bot-uci PRIVATE chess_core)
