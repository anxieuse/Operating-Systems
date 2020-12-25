#include "draw.hpp"

#include <string>

extern "C" void generate(std::string filename)
{
    std::string cmd = "dot source.dot -Tpng -o ";
    cmd += filename + ".png";
    if (system(cmd.c_str()) == -1) {
        perror("Couldn't generate .dot file\n");
    }
}

extern "C" void view(std::string viewer, std::string filename)
{
    std::string cmd = viewer + " " + filename + ".png 2> /dev/null";
    if (system(cmd.c_str()) == -1) {
        perror("Couldn't open image\n");
    }
}
