#include <iostream>

#include "hasher.hpp"
#include "precomputations.hpp"
#include "repl.hpp"

int main()
{
    Precomputations::init_precomputations();
    Hasher::init_hasher();

    std::cout << "\033[?1049h";
    Repl repl;
    repl.run();
    std::cout << "\033[?1049l";
    return 0;
}
