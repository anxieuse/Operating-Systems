#pragma once

#include "Node.hpp"
#include <fstream>

struct TTrie {
    TNode *root;
    int size;

    TTrie();
    void DestructR(TNode *node);

    ~TTrie();

    TNode *Find(TKey *key);

    TNode *Insert(TKey *key, TValue value);

    void KVCopy(TNode *src, TNode *dest);

    bool Delete(TKey *k);

    void Save(std::ofstream &file);

    void enumerate(TNode *node, TNode **nodes, int &index);

    void Load(std::ifstream &file);

    void PrintDefinitions(TNode *node, std::ofstream &out);

    void PrintRelations(TNode *node, std::ofstream &out);
};