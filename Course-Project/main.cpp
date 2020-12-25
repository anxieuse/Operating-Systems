#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include "PatFuncs/Additional.hpp"
#include "PatFuncs/Trie.hpp"

int main()
{
    std::ofstream fout;
    std::ofstream dotout;
    std::ifstream fin;

    char input[MAXLEN];
    TValue value;

    TTrie *trie;
    try {
        trie = new TTrie();
    }
    catch (const std::bad_alloc &e) {
        std::cout << "ERROR: fail to allocate the requested storage space\n";
        exit(1);
    }

    TNode *node;

    while ((std::cin >> input)) {
        if (!std::strcmp(input, "+")) {
            std::cin >> input;
            Lowercase(input);
            std::cin >> value;

            std::cout << (trie->Insert(input, value) ? "OK" : "Exist");
            std::cout << '\n';
        }
        else if (!std::strcmp(input, "-")) {
            std::cin >> input;
            Lowercase(input);

            std::cout << (trie->Delete(input) ? "OK" : "NoSuchWord");
            std::cout << '\n';
        }
        else if (!std::strcmp(input, "!")) {
            std::cin >> input;
            if (!std::strcmp(input, "Save")) {
                std::cin >> input;
                fout.open(input, std::ios::out | std::ios::binary | std::ios::trunc);
                if (!fout.is_open()) {
                    std::cout << "ERROR: can't create file\n";
                    continue;
                }

                trie->Save(fout);
                std::cout << "OK\n";

                fout.close();
            }
            else if (!std::strcmp(input, "Load")) {
                std::cin >> input;
                fin.open(input, std::ios::in | std::ios::binary);
                if (!fin.is_open()) {
                    std::cout << "ERROR: can't open file\n";
                    continue;
                }

                delete trie;
                trie = new TTrie();
                trie->Load(fin);

                std::cout << "OK\n";

                fin.close();
            }
        }
        else if (!std::strcmp(input, "print")) {
            std::string fname, vwr;
            std::cin >> fname >> vwr;

            int pid1 = fork(), childStatus1;
            if (pid1 < 0) {
                perror("fork1 fails\n");
                exit(1);
            }
            else if (pid1 == 0) {
                // creating, fillin .dot
                std::ofstream dot;
                dot.open("source.dot", std::ios::out | std::ios::trunc);
                dot << "digraph {\n";
                trie->PrintDefinitions(trie->root, dot);
                trie->PrintRelations(trie->root, dot);
                dot << "}\n";
                dot.flush(), dot.close();

                // for generating/opening png
                int pid2 = fork(), childStatus2;
                if (pid2 < 0) {
                    perror("fork2 fails\n");
                    exit(1);
                }
                else if (pid2 == 0) {
                    if (execlp("./drawer", "drawer", fname.c_str(), vwr.c_str(), NULL) < 0) {
                        perror("execlp fails");
                        exit(1);
                    }
                }
                waitpid(pid2, &childStatus2, 0);
                if (!WIFEXITED(childStatus2))
                    perror("Something is wrong with 'worker' process\n");
            }
            waitpid(pid1, &childStatus1, 0);
            if (!WIFEXITED(childStatus1))
                perror("Something is wrong with 'dot creating' process\n");
            std::cout.flush();
        }
        else if (!strcmp(input, "exit")) {
            break;
        }
        else {
            Lowercase(input);
            node = trie->Find(input);
            if (!node)
                std::cout << "NoSuchWord";
            else
                std::cout << "OK: " << node->value;
            std::cout << '\n';
        }
    }

    delete trie;

    return 0;
}
