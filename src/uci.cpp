#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "bot.hpp"
#include "color.hpp"
#include "move.hpp"
#include "notation.hpp"

namespace
{
constexpr int MOVES_TO_GO = 30;
constexpr long long SAFETY_MARGIN_MS = 20;

void stop_search(std::atomic<bool>& stop, std::thread& search_thread)
{
    stop = true;
    if (search_thread.joinable())
    {
        search_thread.join();
    }
}

void handle_position(Bot& bot, std::istringstream& input)
{
    std::string type;
    input >> type;

    std::string fen;
    std::string token;
    if (type == "startpos")
    {
        fen = notation::STARTING_POSITION_FEN;
        input >> token;
    }
    else if (type == "fen")
    {
        while (input >> token && token != "moves")
        {
            fen += token + " ";
        }
    }
    else
    {
        return;
    }

    bot.load_position(fen);

    if (token != "moves")
    {
        return;
    }
    while (input >> token)
    {
        const Move move = notation::get_move_from_notation(token, bot.get_move_generator());
        if (move == move::NONE_MOVE)
        {
            std::cerr << "Illegal move in position command: " << token << std::endl;
            return;
        }
        bot.get_board().make_move(move);
    }
}

void handle_go(Bot& bot, std::istringstream& input, std::atomic<bool>& stop,
               std::thread& search_thread)
{
    const auto now = std::chrono::steady_clock::now();
    auto deadline = std::chrono::steady_clock::time_point::max();
    const bool is_white = bot.get_board().get_to_move() == color::WHITE;

    long long own_time = -1;
    long long own_increment = 0;
    int max_depth = Bot::MAX_DEPTH;
    std::string token;
    while (input >> token)
    {
        long long value = 0;
        if (token == "depth")
        {
            input >> max_depth;
        }
        else if (token == "movetime" && input >> value)
        {
            deadline = now + std::chrono::milliseconds(value);
        }
        else if ((token == "wtime" || token == "btime") && input >> value)
        {
            if ((token == "wtime") == is_white)
            {
                own_time = value;
            }
        }
        else if ((token == "winc" || token == "binc") && input >> value)
        {
            if ((token == "winc") == is_white)
            {
                own_increment = value;
            }
        }
    }

    if (own_time >= 0)
    {
        long long think_time = (own_time / MOVES_TO_GO) + (own_increment / 2);
        think_time = std::max(1LL, std::min(think_time, own_time - SAFETY_MARGIN_MS));
        deadline = now + std::chrono::milliseconds(think_time);
    }

    stop = false;
    search_thread = std::thread(
        [&bot, &stop, deadline, max_depth]
        {
            const Move move = bot.go(stop, deadline, max_depth);
            std::cout << ("bestmove " +
                          (move == move::NONE_MOVE ? "0000" : notation::get_move_notation(move)) +
                          "\n")
                      << std::flush;
        });
}
} // namespace

int main()
{
    Bot bot;
    bot.load_position(std::string(notation::STARTING_POSITION_FEN));
    std::atomic<bool> stop = false;
    std::thread search_thread;

    std::string line;
    while (std::getline(std::cin, line))
    {
        std::istringstream input(line);
        std::string command;
        input >> command;

        if (command == "uci")
        {
            std::cout << "id name ChessBot" << std::endl;
            std::cout << "id author Jan Rousek" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (command == "isready")
        {
            std::cout << "readyok" << std::endl;
        }
        else if (command == "position")
        {
            stop_search(stop, search_thread);
            handle_position(bot, input);
        }
        else if (command == "go")
        {
            stop_search(stop, search_thread);
            handle_go(bot, input, stop, search_thread);
        }
        else if (command == "stop")
        {
            stop_search(stop, search_thread);
        }
        else if (command == "quit")
        {
            break;
        }
    }

    stop_search(stop, search_thread);
    return 0;
}
