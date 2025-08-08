#ifndef REPL_H
#define REPL_H

#include <string>
#include <thread>

#include "bot.hpp"
#include "types.hpp"

class Repl
{
  public:
    void run();

  private:
    enum class Mode
    {
        IDLE,
        PLAY,
        ANALYSE,
        FINISHED
    };

    DisplayMode display_mode = DisplayMode::LETTERS;
    std::string info_line = "";
    Mode mode = Mode::IDLE;
    Bot bot;
    bool stop_analysis = false;
    std::thread analysis_thread;
    int thinking_time;
    std::string old_board_string;

    std::vector<std::string> split_args(std::string args_string);
    std::string join_args(std::vector<std::string>& args);
    void handle_command(std::string command, std::vector<std::string> args);
    void clear_screen();
    void print_board();
    void update_screen();
    void make_bot_move();
    bool is_game_over();
    std::string get_mode_symbol();
};

#endif
