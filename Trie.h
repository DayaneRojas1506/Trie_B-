#include <iostream>
#include <unordered_map>
#include <string>
using namespace std;

class TrieNode {
public:
    unordered_map<char, TrieNode*> children;
    bool is_end_of_word;
    TrieNode* prev;
    TrieNode* next;

    TrieNode() {
        is_end_of_word = false;
        prev = nullptr;
        next = nullptr;
    }
};

class Trie {
public:
    TrieNode* root;

    Trie() {
        root = new TrieNode();
    }

    void insert(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                node->children[c] = new TrieNode();
                if (node->next != nullptr) {
                    node->children[c]->prev = node;
                    node->children[c]->next = node->next;
                    node->next->prev = node->children[c];
                }
                node->next = node->children[c];
            }
            node = node->children[c];
        }
        node->is_end_of_word = true;
    }

    bool search(string word) {
        TrieNode* node = root;
        for (char c : word) {
            if (node->children.find(c) == node->children.end()) {
                return false;
            }
            node = node->children[c];
        }
        return node != nullptr && node->is_end_of_word;
    }

    bool starts_with(string prefix) {
        TrieNode* node = root;
        for (char c : prefix) {
            if (node->children.find(c) == node->children.end()) {
                return false;
            }
            node = node->children[c];
        }
        return node != nullptr;
    }

    bool deleteWord(string word) {
        return deleteHelper(root, word, 0);
    }

private:
    bool deleteHelper(TrieNode* node, string word, int index) {
        if (index == word.length()) {
            if (!node->is_end_of_word) {
                return false;
            }
            node->is_end_of_word = false;
            return node->children.empty();
        }

        char c = word[index];
        if (node->children.find(c) == node->children.end()) {
            return false;
        }

        bool shouldDeleteNode = deleteHelper(node->children[c], word, index + 1);

        if (shouldDeleteNode) {
            TrieNode* child = node->children[c];
            node->children.erase(c);
            if (node->prev != nullptr) {
                node->prev->next = child->next;
            }
            if (child->next != nullptr) {
                child->next->prev = node->prev;
            }
            delete child;
            return node->children.empty();
        }

        return false;
    }
};

int main() {
    Trie trie;
    trie.insert("apple");
    trie.insert("banana");
    trie.insert("orange");
    trie.insert("app");
    trie.insert("orangeade");

    cout << std::boolalpha;
    cout << trie.search("apple") << endl;   // Output: true
    cout << trie.search("app") << endl;     // Output: true
    cout << trie.search("or") << endl;      // Output: false
    cout << trie.starts_with("or") << endl; // Output: true

    trie.deleteWord("apple");
    cout << trie.search("apple") << endl;   // Output: false

    return 0;
}

