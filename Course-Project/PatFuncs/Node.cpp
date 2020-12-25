#include "Node.hpp"
#include <cstring>

void TNode::Initialize(int b, TKey *k, TValue v, TNode *l, TNode *r)
{
    bit = b;
    if (k) {
        key = new char[strlen(k) + 1];
        strcpy(key, k);
    }
    else
        key = k;
    value = v;
    left = l;
    right = r;
}

TNode::TNode()
{
    Initialize(-1, 0, 0, this, this);
}

TNode::TNode(int b, TKey *k, TValue v)
{
    Initialize(b, k, v, this, this);
}

TNode::TNode(int b, TKey *k, TValue v, TNode *l, TNode *r)
{
    Initialize(b, k, v, l, r);
}

TNode::~TNode()
{
    delete[] key;
}


