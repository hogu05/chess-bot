#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "bot.hpp"

namespace
{
constexpr int BENCH_DEPTH = 8;

const std::vector<std::string> positions = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};
} // namespace

int main()
{
    std::cout << std::left;
    std::cout << std::setw(8) << "Test";
    std::cout << std::setw(12) << "Nodes";
    std::cout << std::setw(10) << "Time(ms)";
    std::cout << "kN/s" << std::endl;

    std::uint64_t total_nodes = 0;
    std::uint64_t total_time = 0;

    for (std::size_t test_id = 0; test_id < positions.size(); test_id++)
    {
        Bot bot;
        bot.load_position(positions[test_id]);
        std::atomic<bool> stop = false;

        const auto start = std::chrono::steady_clock::now();
        bot.go(stop, std::chrono::steady_clock::time_point::max(), BENCH_DEPTH);
        const auto end = std::chrono::steady_clock::now();

        const std::uint64_t nodes = bot.get_searched_nodes();
        const std::uint64_t time =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        total_nodes += nodes;
        total_time += time;

        std::cout << std::setw(8) << test_id;
        std::cout << std::setw(12) << nodes;
        std::cout << std::setw(10) << time;
        std::cout << (time > 0 ? nodes / time : 0) << std::endl;
    }

    std::cout << std::string(40, '-') << std::endl;
    std::cout << "Total nodes: " << total_nodes << " | ";
    std::cout << "Total time: " << total_time << "ms" << " | ";
    std::cout << "Average speed: " << (total_time > 0 ? total_nodes / total_time : 0) << "kN/s"
              << std::endl;

    return 0;
}
