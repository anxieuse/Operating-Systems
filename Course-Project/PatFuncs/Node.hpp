#pragma once

const int MAXLEN = 256;
typedef unsigned long long TValue;
typedef char TKey;

struct TNode
{
    int id = -1;
    int bit;

    TKey *key;
    TValue value;

    TNode *left;
    TNode *right;

    void Initialize(int b, TKey *k, TValue v, TNode *l, TNode *r);

    TNode();

    TNode(int b, TKey *k, TValue v);

    TNode(int b, TKey *k, TValue v, TNode *l, TNode *r);

    ~TNode();
};

