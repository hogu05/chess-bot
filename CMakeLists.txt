cmake_minimum_required(VERSION 3.28)
project(chess-bot)

set(CMAKE_CXX_STANDARD 26)

add_executable(chess-bot main.cpp board.cpp piece.cpp move_generator.cpp move.cpp precomputations.cpp directions.cpp bitboard.cpp position_info.cpp)
