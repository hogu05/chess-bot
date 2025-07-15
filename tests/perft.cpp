#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "board.hpp"
#include "move_generator.hpp"
#include "precomputations.hpp"

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
    uint64_t time;
    bool passed;
};

std::vector<TestResult> run_tests(std::vector<TestCase> tests)
{
    std::vector<TestResult> results;

    Precomputations::init_precomputations();

    Board board;
    MoveGenerator generator(board);

    for (int i = 0; i < tests.size(); i++)
    {
        TestCase &test = tests[i];

        board.load_position(test.fen);

        auto start = std::chrono::high_resolution_clock::now();
        std::uint64_t nodes = generator.perft(test.depth);
        auto stop = std::chrono::high_resolution_clock::now();

        std::uint64_t time =
            std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

        results.emplace_back(i, nodes, test.expected_nodes, time, nodes == test.expected_nodes);
    }

    return results;
}

bool all_passed(std::vector<TestResult> results)
{
    return std::all_of(results.begin(), results.end(),
                       [](const TestResult &result) { return result.passed; });
}

void print_results(std::vector<TestResult> results)
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

    for (const TestResult &result : results)
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

int main()
{
    std::vector<TestCase> tests = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", 4, 4085603},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 6, 11030083},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5, 15833292},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194}};

    std::vector<TestResult> results = run_tests(tests);
    print_results(results);

    return all_passed(results) ? 0 : 1;
}
