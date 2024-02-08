
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;
// Node class
class Node {
public:
	int t; // Order of the B+ tree
	vector<string> keys; // Keys in the node
	vector<vector<int>> values;
	vector<Node*> child_ptr; // Pointers to child nodes
	bool leaf; // Boolean to check if the node is a leaf
	int n; // Current number of keys
	Node* ptr2next; // Pointer to the next node
	// Node constructor
	Node(int _t, Node* _ptr2next = NULL) {
		t = _t;
		ptr2next = _ptr2next;
		leaf = true;
		keys.resize(2*t-1);
		values.resize(2*t-1);
		child_ptr.resize(2*t);
		n = 0;
	}
	// Function to insert a key in a non-full node
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
	// Function to split the child
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
	// Function to print the tree
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
	// Function to search a key in the tree
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
};

class BTree {
public:
	Node* root; // Root of the B+ tree
	int t; // Order of the B+ tree
	// BTree constructor
	BTree(int _t) {
		root = new Node(_t);
		root->leaf = true;
	}
	// Function to insert a key in the tree
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
	// Function to print the tree
	void print() {
		root->print();
	}
	// Function to search a key in the tree
	Node* search(string k, int v) {
		return (root == NULL)? NULL : root->search(k, v);
	}
};
// Function to print the tree
void printTree(BTree* tree) {
	tree->print();
}

int main() {
	int record_len = 3;
	BTree* bplustree = new BTree(record_len);
	bplustree->insert("5", 33);
	bplustree->insert("15", 21);
	bplustree->insert("25", 31);
	bplustree->insert("35", 41);
	bplustree->insert("45", 10);

	printTree(bplustree);

	if (bplustree->search("5", 34) != NULL) {
		cout << "Found" << endl;
	} else {
		cout << "Not found" << endl;
	}

	return 0;
}
