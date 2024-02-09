#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Node {
public:
    int t; // Orden
    vector<string> keys;
    vector<vector<int>> values;
    vector<Node*> child_ptr;
    bool leaf;
    int n;
    Node* ptr2next;

    Node(int _t, Node* _ptr2next = NULL) {
        t = _t;
        ptr2next = _ptr2next;
        leaf = true;
        keys.resize(2*t-1);
        values.resize(2*t-1);
        child_ptr.resize(2*t);
        n = 0;
    }

    void insertNonFull(string k, int v) {
        int i = n-1;
        if (leaf) {
            keys.insert(keys.begin()+n, k);
            values.insert(values.begin()+n, vector<int>(1, v));
            n += 1;
            while (i>=0 && keys[i]>k) {
                swap(keys[i], keys[i+1]);
                swap(values[i], values[i+1]);
                i -= 1;
            }
        } else {
            while (i>=0 && keys[i]>k) {
                i -= 1;
            }
            i += 1;
            if (child_ptr[i]->n == 2*t-1) {
                splitChild(i);
                if (keys[i] < k) {
                    i += 1;
                }
            }
            child_ptr[i]->insertNonFull(k, v);
        }
    }

    void splitChild(int i) {
        Node* y = child_ptr[i];
        Node* z = new Node(y->t, y->ptr2next);
        child_ptr.insert(child_ptr.begin()+i+1, z);
        keys.insert(keys.begin()+i, y->keys[t-1]);
        values.insert(values.begin()+i, y->values[t-1]);
        y->ptr2next = z;
        z->leaf = y->leaf;
        z->n = t-1;
        y->n = t-1;
        for (int j=0; j<t-1; j++) {
            z->keys[j] = y->keys[j+t];
            z->values[j] = y->values[j+t];
        }
        if (!y->leaf) {
            for (int j=0; j<t; j++) {
                z->child_ptr[j] = y->child_ptr[j+t];
            }
        }
        n += 1;
    }

    void print() {
        for (int i=0; i<n; i++) {
            if (!leaf) {
                child_ptr[i]->print();
            }
            cout << "['" << keys[i] << "']" << endl;
        }
        if (!leaf) {
            child_ptr[n]->print();
        }
    }

    Node* search(string k, int v) {
        int i = 0;
        while (i<n && k>keys[i]) {
            i += 1;
        }
        if (keys[i] == k) {
            for (int j = 0; j < values[i].size(); j++) {
                if (values[i][j] == v) {
                    return this;
                }
            }
        }
        if (leaf) {
            return NULL;
        } else {
            return child_ptr[i]->search(k, v);
        }
    }

    void remove(string k, int v) {
        int idx = findKeyIndex(k);
        if (idx < n && keys[idx] == k) {
            // Encontramos la clave en este nodo
            int valIdx = findValueIndex(idx, v);
            if (valIdx != -1) {
                // Eliminar el valor de la lista de valores
                values[idx].erase(values[idx].begin() + valIdx);
                if (values[idx].empty()) {
                    // Si la lista de valores está vacía, eliminar la clave
                    keys.erase(keys.begin() + idx);
                    values.erase(values.begin() + idx);
                    n--;
                }
                return;
            }
        }
        if (!leaf) {
            bool flag = (idx == n);
            if (child_ptr[idx]->n < t) {
                fill(idx);
            }
            if (flag && idx > n) {
                child_ptr[idx - 1]->remove(k, v);
            } else {
                child_ptr[idx]->remove(k, v);
            }
        }
    }

private:
    int findKeyIndex(string k) {
        int idx = 0;
        while (idx < n && keys[idx] < k) {
            ++idx;
        }
        return idx;
    }

    int findValueIndex(int idx, int v) {
        for (int i = 0; i < values[idx].size(); ++i) {
            if (values[idx][i] == v) {
                return i;
            }
        }
        return -1;
    }

    void fill(int idx) {
        if (idx != 0 && child_ptr[idx - 1]->n >= t) {
            borrowFromPrev(idx);
        } else if (idx != n && child_ptr[idx + 1]->n >= t) {
            borrowFromNext(idx);
        } else {
            if (idx != n) {
                merge(idx);
            } else {
                merge(idx - 1);
            }
        }
    }

    void borrowFromPrev(int idx) {
        Node *child = child_ptr[idx];
        Node *sibling = child_ptr[idx - 1];
        for (int i = child->n - 1; i >= 0; --i) {
            child->keys[i + 1] = child->keys[i];
        }
        child->keys[0] = keys[idx - 1];
        keys[idx - 1] = sibling->keys[sibling->n - 1];
        for (int i = child->n - 1; i >= 0; --i) {
            child->values[i + 1] = child->values[i];
        }
        child->values[0] = sibling->values[sibling->n - 1];
        if (!child->leaf) {
            for (int i = child->n; i >= 0; --i) {
                child->child_ptr[i + 1] = child->child_ptr[i];
            }
            child->child_ptr[0] = sibling->child_ptr[sibling->n];
        }
        ++child->n;
        --sibling->n;
    }

    void borrowFromNext(int idx) {
        Node *child = child_ptr[idx];
        Node *sibling = child_ptr[idx + 1];
        child->keys[child->n] = keys[idx];
        keys[idx] = sibling->keys[0];
        child->values[child->n] = sibling->values[0];
        if (!child->leaf) {
            child->child_ptr[child->n + 1] = sibling->child_ptr[0];
        }
        ++child->n;
        for (int i = 1; i < sibling->n; ++i) {
            sibling->keys[i - 1] = sibling->keys[i];
        }
        for (int i = 1; i < sibling->n; ++i) {
            sibling->values[i - 1] = sibling->values[i];
        }
        if (!sibling->leaf) {
            for (int i = 1; i <= sibling->n; ++i) {
                sibling->child_ptr[i - 1] = sibling->child_ptr[i];
            }
        }
        --sibling->n;
    }

    void merge(int idx) {
        Node *child = child_ptr[idx];
        Node *sibling = child_ptr[idx + 1];
        child->keys[t - 1] = keys[idx];
        for (int i = 0; i < sibling->n; ++i) {
            child->keys[i + t] = sibling->keys[i];
        }
        if (!child->leaf) {
            for (int i = 0; i <= sibling->n; ++i) {
                child->child_ptr[i + t] = sibling->child_ptr[i];
            }
        }
        for (int i = idx + 1; i < n; ++i) {
            keys[i - 1] = keys[i];
        }
        for (int i = idx + 2; i <= n; ++i) {
            child_ptr[i - 1] = child_ptr[i];
        }
        child->n += sibling->n + 1;
        --n;
        delete sibling;
    }
};

class BTreeplus {
public:
    Node* root;
    int t; // orden

    BTreeplus(int _t) {
        root = new Node(_t);
        root->leaf = true;
        t = _t;
    }

    void insert(string k, int v) {
        Node* r = root;
        if (r->n == 2*t-1) {
            Node* s = new Node(t);
            root = s;
            s->child_ptr[0] = r;
            s->splitChild(0);
            s->insertNonFull(k, v);
        } else {
            r->insertNonFull(k, v);
        }
    }

    void print() {
        root->print();
    }

    Node* search(string k, int v) {
        return (root == NULL)? NULL : root->search(k, v);
    }

    void remove(string k, int v) {
        if (root == nullptr) {
            return;
        }
        root->remove(k, v);
        if (root->n == 0) {
            Node* tmp = root;
            if (root->leaf) {
                root = nullptr;
            } else {
                root = root->child_ptr[0];
            }
            delete tmp;
        }
    }
};

void printTree(BTreeplus* tree) {
    tree->print();
}

int main() {
    int len = 3;
    BTreeplus* bplustree = new BTreeplus(len);
    bplustree->insert("5", 33);
    bplustree->insert("15", 21);
    bplustree->insert("25", 31);
    bplustree->insert("35", 41);
    bplustree->insert("45", 10);

    printTree(bplustree);

    if (bplustree->search("5", 34) != NULL) {
        cout << "Se encontro" << endl;
    } else {
        cout << "No se encontro" << endl;
    }

    bplustree->remove("15", 21);
    printTree(bplustree);

    return 0;
}
