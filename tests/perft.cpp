#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "board.hpp"
#include "move_generator.hpp"

namespace
{
struct TestCase
{
    std::string fen;
    int depth;
    std::uint64_t expected_nodes;
};

struct TestResult
{
    int test_id;
    std::uint64_t actual_nodes;
    std::uint64_t expected_nodes;
    std::uint64_t time;
    bool passed;
};

std::vector<TestResult> run_tests(const std::vector<TestCase>& tests)
{
    std::vector<TestResult> results;

    Board board;
    MoveGenerator generator(board);

    int test_id = 0;
    for (const TestCase& test : tests)
    {
        board.load_position(test.fen);

        const auto start = std::chrono::steady_clock::now();
        const std::uint64_t nodes = generator.perft(test.depth);
        const auto stop = std::chrono::steady_clock::now();

        results.emplace_back(
            test_id, nodes, test.expected_nodes,
            std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(),
            nodes == test.expected_nodes);
        test_id++;
    }

    return results;
}

bool all_passed(const std::vector<TestResult>& results)
{
    return std::all_of(results.begin(), results.end(),
                       [](const TestResult& result) { return result.passed; });
}

void print_results(const std::vector<TestResult>& results)
{
    std::cout << std::left;
    std::cout << std::setw(8) << "Test";
    std::cout << std::setw(12) << "Nodes";
    std::cout << std::setw(12) << "Expected";
    std::cout << std::setw(10) << "Time(ms)";
    std::cout << std::setw(12) << "kN/s";
    std::cout << "Result" << std::endl;

    std::uint64_t total_nodes = 0;
    std::uint64_t total_time = 0;

    for (const TestResult& result : results)
    {
        total_nodes += result.actual_nodes;
        total_time += result.time;

        std::cout << std::left;
        std::cout << std::setw(8) << result.test_id;
        std::cout << std::setw(12) << result.actual_nodes;
        std::cout << std::setw(12) << result.expected_nodes;
        std::cout << std::setw(10) << result.time;
        std::cout << std::setw(12) << (result.time > 0 ? result.actual_nodes / result.time : 0);
        std::cout << (result.passed ? "✓" : "✗") << std::endl;
    }

    std::cout << std::string(60, '-') << std::endl;

    if (all_passed(results))
    {
        std::cout << "All tests passed!" << std::endl;
        std::cout << "Total nodes: " << total_nodes << " | ";
        std::cout << "Total time: " << total_time << "ms" << " | ";
        std::cout << "Average speed: " << total_nodes / total_time << "kN/s" << std::endl;
    }
    else
    {
        std::cout << "Some tests failed!" << std::endl;
    }
}
} // namespace

int main()
{
    const std::vector<TestCase> tests = {
        {.fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
         .depth = 7,
         .expected_nodes = 3195901860},
        {.fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ",
         .depth = 5,
         .expected_nodes = 193690690},
        {.fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
         .depth = 7,
         .expected_nodes = 178633661},
        {.fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
         .depth = 6,
         .expected_nodes = 706045033},
        {.fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
         .depth = 5,
         .expected_nodes = 89941194}};

    const std::vector<TestResult> results = run_tests(tests);
    print_results(results);

    return all_passed(results) ? 0 : 1;
}
