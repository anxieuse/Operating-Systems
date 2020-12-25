#include "draw.hpp"
#include <dlfcn.h>
#include <bits/stdc++.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cout << "Not enough arguments for drawer-program\n";
        return 0;
    }

    std::string fname = argv[1];
    std::string vwr = argv[2];

    void *library_handler = NULL;
    library_handler = dlopen("./libdraw.so", RTLD_LAZY);
    if (!library_handler) {
        fprintf(stderr, "dlopen() error: %s\n", dlerror());
        exit(1);
    }

    void (*generate)(std::string filename);
    void (*view)(std::string viewer, std::string filename);

    generate = (void (*)(std::string))dlsym(library_handler, "generate");
    std::cout << dlerror() << std::endl;
    
    view = (void (*)(std::string, std::string))dlsym(library_handler, "view");
    std::cout << dlerror() << std::endl;

    generate(fname);
    view(vwr, fname);

    dlclose(library_handler);
    return 0;
}