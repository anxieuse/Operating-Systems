#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

class Tree {
private:
    struct Node;

public:
    Tree() = default;

    ~Tree()
    {
        deleteTree(root);
    }

    bool find(const int &id)
    {
        Node *temp = root;
        while (temp != nullptr) {
            if (temp->id == id)
                break;
            if (id > temp->id)
                temp = temp->right;
            if (id < temp->id)
                temp = temp->left;
        }
        return temp != nullptr;
    }

    void insert(int id)
    {
        if (root == nullptr) {
            root = new Node(id);
            return;
        }
        Node *temp = root;
        while (temp != nullptr) {
            if (id == temp->id)
                break;
            if (id < temp->id) {
                if (temp->left == nullptr) {
                    temp->left = new Node(id);
                    break;
                }
                temp = temp->left;
            }
            if (id > temp->id) {
                if (temp->right == nullptr) {
                    temp->right = new Node(id);
                    break;
                }
                temp = temp->right;
            }
        }
    }

    void erase(int id)
    {
        Node *prev_id = nullptr;
        Node *temp = root;
        while (temp != nullptr) {
            if (id == temp->id) {
                if (prev_id == nullptr) {
                    root = nullptr;
                }
                else {
                    if (prev_id->left == temp)
                        prev_id->left = nullptr;
                    else
                        prev_id->right = nullptr;
                }
                deleteTree(temp);
            }
            else if (id < temp->id) {
                prev_id = temp;
                temp = temp->left;
            }
            else if (id > temp->id) {
                prev_id = temp;
                temp = temp->right;
            }
        }
    }

    std::vector<int> getNodesList() const
    {
        std::vector<int> result;
        getNodesList(root, result);
        return result;
    }

    void dictInsertWord(int id, std::string word, int value)
    {
        Node *node = getNodeById(root, id);
        node->dictionary[word] = value;
    }

    void dictGetWord(int id, std::string word)
    {
        Node *node = getNodeById(root, id);
        if (node->dictionary.find(word) == node->dictionary.end())
            std::cout << "'" << word << "' not found" << '\n';
        else
            std::cout << node->dictionary[word] << '\n';
    }

private:
    struct Node {
        Node(int id) : id(id) {}
        int id = 0;
        Node *left = nullptr;
        Node *right = nullptr;
        std::unordered_map<std::string, int> dictionary;
    };

    Node *root = nullptr;

    Node *getNodeById(Node *root, int id)
    {
        if (root == nullptr || root->id == id) {
            return root;
        }

        if (root->id < id) {
            return getNodeById(root->right, id);
        }

        return getNodeById(root->left, id);
    }

    void getNodesList(Node *node, std::vector<int> &v) const
    {
        if (node == nullptr)
            return;
        getNodesList(node->left, v);
        v.push_back(node->id);
        getNodesList(node->right, v);
    }

    void deleteTree(Node *node)
    {
        if (node == nullptr)
            return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
};