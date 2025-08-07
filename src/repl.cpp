#include "repl.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "bot.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "position_info.hpp"
#include "precomputations.hpp"
#include "types.hpp"

std::vector<std::string> Repl::split_args(std::string args_string)
{
    std::istringstream args_stream(args_string);
    return {std::istream_iterator<std::string>(args_stream), std::istream_iterator<std::string>()};
}

std::string Repl::join_args(std::vector<std::string>& args)
{
    std::ostringstream oss;
    for (int i = 0; i < args.size(); i++)
    {
        oss << args[i];
        if (i != args.size() - 1)
        {
            oss << " ";
        }
    }
    return oss.str();
}

void Repl::handle_command(std::string command, std::vector<std::string> args)
{
    if (state == State::IDLE)
    {
        if (command == "load")
        {
            std::string fen = args.empty() ? Board::STARTING_POSITION_FEN : join_args(args);
            bot.load_position(fen);
            info_line = "Position loaded";
            return;
        }

        if (command == "move")
        {
            if (args.empty())
            {
                info_line = "No move specified";
                return;
            }

            std::string move_notation = args[0];
            Move_t move = Move::get_move_from_notation(move_notation, bot.get_move_generator());

            if (move == Move::NONE_MOVE)
            {
                info_line = "Illegal move: " + move_notation;
                return;
            }

            bot.get_board().make_move(move);
            info_line = "Played move: " + move_notation;
            return;
        }

        if (command == "go")
        {
            state = State::ANALYSING;
            stop_analysis = false;
            std::function<void(int, Move_t, int)> callback =
                [this](int depth, Move_t best_move, int evaluation)
            {
                info_line = "Evaluation: " + std::to_string(evaluation / 100) + " | " +
                            "Best move: " + Move::get_move_notation(best_move) + " | " +
                            "Depth: " + std::to_string(depth);

                std::cout << "\033[s"
                          << "\033[A"
                          << "\033[2K"
                          << "\033[G" << info_line << "\033[u" << std::flush; // Update info line
            };

            stop_analysis = false;
            old_board_string = bot.get_board().to_string(display_mode);
            info_line = "Analysis startig";
            analysis_thread = std::thread([this, callback]() { bot.go(callback, stop_analysis); });
            analysis_thread.detach();
            return;
        }

        if (command == "play")
        {
            thinking_time = 3000;
            if (!args.empty())
            {
                thinking_time = std::stoi(args[0]);
            }
            state = State::PLAYING;

            make_bot_move();
            return;
        }
    }

    if (command == "quit")
    {
        if (state == State::ANALYSING)
        {
            stop_analysis = true;
        }

        state = State::FINISHED;
        return;
    }

    if (command == "display")
    {
        if (args.empty())
        {
            info_line = "No display mode specified";
            return;
        }

        std::string mode = args[0];
        if (mode == "letters")
        {
            display_mode = DisplayMode::LETTERS;
            info_line = "Display mode changed to letters";
        }
        else if (mode == "unicode")
        {
            display_mode = DisplayMode::UNICODE;
            info_line = "Display mode changed to unicode";
        }
        else
        {
            info_line = "Unknown display mode: " + mode;
        }
        return;
    }

    if (command == "stop")
    {
        if (state == State::ANALYSING)
        {
            stop_analysis = true;
            info_line = "Analysis stopped";
        }

        if (state == State::PLAYING)
        {
            info_line = "Game stopped";
        }

        if (state != State::IDLE)
        {
            state = State::IDLE;
            return;
        }
    }

    if (command == "help")
    {
        if (state == State::IDLE)
        {
            info_line =
                "load <FEN>         - Load a position from FEN (default: starting position)\n";
            info_line +=
                "move <notation>    - Make a move using standard algebraic notation (e.g., e2e4)\n";
            info_line += "go                 - Start engine analysis\n";
            info_line += "play [time]        - Start playing against a bot from current position, "
                         "optionally specifying thinking time in milliseconds (default: 3000ms)\n";
            info_line += "display <mode>     - Change the display mode (letters or unicode)\n";
            info_line += "stop               - Stop the current analysis or game\n";
            info_line += "quit               - End the programme\n";
            info_line += "help               - Show commands for the current mode";
        }

        if (state == State::PLAYING)
        {
            info_line =
                "<notation>         - Make a move using standard algebraic notation (e.g., e2e4)\n";
            info_line += "stop               - Stop the game";
        }

        if (state == State::ANALYSING)
        {
            info_line = "stop               - Stop the analysis";
        }

        return;
    }

    if (state == State::PLAYING)
    {
        std::string move_notation = command;
        Move_t move = Move::get_move_from_notation(move_notation, bot.get_move_generator());

        if (move == Move::NONE_MOVE)
        {
            info_line = "Illegal move: " + move_notation;
            return;
        }

        bot.get_board().make_move(move);
        make_bot_move();
        return;
    }

    if (state == State::IDLE)
    {

        info_line = "Unknown command: " + command;
    }
}

void Repl::run()
{
    Precomputations::init_precomputations();
    bot.load_position(Board::STARTING_POSITION_FEN);
    info_line = "Welcome to the Chess Bot, type 'help' for a list of commands";

    for (;;)
    {
        update_screen();

        std::string input;
        std::getline(std::cin, input);
        std::istringstream input_stream(input);

        std::string command;
        input_stream >> command;

        std::string args_string;
        std::getline(input_stream, args_string);

        std::vector<std::string> args = split_args(args_string);

        handle_command(command, args);

        if (state == State::FINISHED)
        {
            break;
        }
    }
}

void Repl::clear_screen()
{
    std::cout << "\033[2J\033[H";
}

void Repl::print_board()
{
    if (state == State::ANALYSING)
    {
        std::cout << old_board_string;
    }
    else
    {
        std::cout << bot.get_board().to_string(display_mode);
    }
    std::cout << std::string(2, ' ') << get_state_symbol();
}

void Repl::update_screen()
{
    clear_screen();
    print_board();
    std::cout << std::endl << std::endl;
    std::cout << info_line << std::endl;
    std::cout << "> ";
}

void Repl::make_bot_move()
{
    info_line = "Bot is thinking...";
    if (is_game_over())
    {
        return;
    }
    update_screen();

    Move_t move = bot.play(thinking_time);
    bot.get_board().make_move(move);
    info_line = "Bot played: " + Move::get_move_notation(move) + " | It's your turn";
    is_game_over();
    update_screen();
}

bool Repl::is_game_over()
{
    bool is_game_over = false;
    if (bot.get_move_generator().is_checkmate())
    {
        Move_t loser_color = PositionInfo::get_to_move(bot.get_board().position_info);
        info_line = "Checkmate, ";
        info_line += (loser_color == Piece::WHITE ? "black" : "white");
        info_line += " has won the game";
        is_game_over = true;
    }

    if (bot.get_move_generator().is_stalemate())
    {
        info_line = "Stalemate, the game is a draw";
        state = State::IDLE;
        is_game_over = true;
    }

    if (is_game_over)
    {
        state = State::IDLE;
        update_screen();
    }

    return is_game_over;
}

std::string Repl::get_state_symbol()
{
    switch (state)
    {
    case State::IDLE:
        return "I";
    case State::ANALYSING:
        return "A";
    case State::PLAYING:
        return "P";
    default:
        return "";
    }
}
