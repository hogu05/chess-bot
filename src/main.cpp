#include <iostream>

#include "repl.hpp"

int main()
{
    std::cout << "\033[?1049h";
    Repl repl;
    repl.run();
    std::cout << "\033[?1049l";
    return 0;
}
