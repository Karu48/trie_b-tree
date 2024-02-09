#include <iostream>
using namespace std;

// Forward declaration para evitar problemas en la declaracion de friend class
template <typename T>
class BPlusTree;


// Clase BPlusNode
////////////////////////////////////////
template <typename T>
class BPlusNode {
public:
    T* keys;
    int t; // Minimum degree
    BPlusNode** C; // Child pointers
    int n; // Current number of keys
    bool leaf; // Is true when node is leaf
    BPlusNode* next; // Pointer to the next leaf node, importante en B+ Trees

    BPlusNode(int _t, bool _leaf);

    void insertNonFull(T k);
    void splitChild(int ind, BPlusNode* y);
    void traverse();
    BPlusNode* search(T k);
    int findKey(T k);
    void remove(T k);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    T getPred(int idx);
    T getSucc(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);

    friend class BPlusTree<T>;
};


// Clase BPlusTree
////////////////////////////////////////
template <typename T>
class BPlusTree {
public:
    BPlusNode<T>* root;
    int t;

    BPlusTree(int _t) {
        root = NULL;
        t = _t;
    }

    void insert(T k);
    void remove(T k);
    void traverse();
    BPlusNode<T>* search(T k) {
        return (root == NULL) ? NULL : root->search(k);
    }
};

// Implementacion de metodos de BPlusNode  y BPlusTree
////////////////////////////////////////


// Constructor de BPlusNode
////////////////////////////////////////
template <typename T>
BPlusNode<T>::BPlusNode(int _t, bool _leaf) : t(_t), leaf(_leaf), n(0), next(nullptr) {
    keys = new T[2*t-1];
    C = new BPlusNode<T>*[2*t];
}


// Traverse
////////////////////////////////////////

template <typename T>
void BPlusNode<T>::traverse() {
    // For non-leaf nodes, escanea todos los hijos y sus claves (de los subtrees)
    int i;
    for (i = 0; i < n; i++) {
        if (!leaf) {
            C[i]->traverse();
        }
        cout << " " << keys[i];
    }
    // Print the subtree con su ultimo hijo
    if (!leaf) {
        C[i]->traverse();
    }
}

template <typename T>
void BPlusTree<T>::traverse() {
    if (root != NULL) {
        // Find the leftmost leaf
        BPlusNode<T>* curr = root;
        while (!curr->leaf) {
            curr = curr->C[0];
        }
        // Now traverse usando next pointers en leaf nodes
        while (curr != NULL) {
            for (int i = 0; i < curr->n; i++) {
                cout << " " << curr->keys[i];
            }
            curr = curr->next;
        }
    }
}


// Search
////////////////////////////////////////

template <typename T>
BPlusNode<T>* BPlusNode<T>::search(T k) {
    int i = 0;
    while (i < n && k > keys[i]) i++;
    if (keys[i] == k) return this;
    if (leaf) return nullptr;
    return C[i]->search(k);
}


// Insert
////////////////////////////////////////  
template <typename T>
void BPlusNode<T>::insertNonFull(T k) {
    int i = n-1;
    if (leaf) {
        while (i >= 0 && keys[i] > k) {
            keys[i+1] = keys[i];
            i--;
        }
        keys[i+1] = k;
        n = n+1;
    } else {
        while (i >= 0 && keys[i] > k) i--;
        if (C[i+1]->n == 2*t-1) {
            splitChild(i+1, C[i+1]);
            if (keys[i+1] < k) i++;
        }
        C[i+1]->insertNonFull(k);
    }
}

template <typename T>
void BPlusNode<T>::splitChild(int ind, BPlusNode<T>* y) {
    // Crear new node para store (t-1) keys de y
    BPlusNode<T>* z = new BPlusNode<T>(y->t, y->leaf);
    z->n = t - 1;

    // Copiar last (t-1) keys de y a z
    for (int i = 0; i < t-1; i++) {
        z->keys[i] = y->keys[i + t];
    }

    // Si y no es leaf, copiar t children de y a z
    if (!y->leaf) {
        for (int i = 0; i < t; i++) {
            z->C[i] = y->C[i + t];
        }
    } else {
        // Si y es leaf, z es el next node de y
        z->next = y->next;
        y->next = z;
    }

    // Reducimos number of keys en y
    y->n = t - 1;

    // Crear space para new child
    for (int i = n; i >= ind+1; i--) {
        C[i + 1] = C[i];
    }

    // Link new child a este nodo
    C[ind + 1] = z;

    // Mover all keys greater que y's middle key one position ahead
    for (int i = n-1; i >= ind; i--) {
        keys[i + 1] = keys[i];
    }

    // Mover y's middle key a este nodo
    keys[ind] = y->keys[t-1];

    // Incrementar count of keys en este nodo
    n = n + 1;
}


template <typename T>
void BPlusTree<T>::insert(T k) {
    if (root == NULL) {
        root = new BPlusNode<T>(t, true);
        root->keys[0] = k;
        root->n = 1;
    } else {
        if (root->n == 2*t-1) {
            BPlusNode<T>* s = new BPlusNode<T>(t, false);
            s->C[0] = root;
            s->splitChild(0, root);
            int i = 0;
            if (s->keys[0] < k) i++;
            s->C[i]->insertNonFull(k);
            root = s;
        } else root->insertNonFull(k);
    }
}


// Remove Methods
////////////////////////////////////////

template <typename T>
void BPlusTree<T>::remove(T k) {
    if (!root) {
        cout << "The tree is empty\n";
        return;
    }

    // Start from the root and remove the key from the leaf node
    root->remove(k);

    // If the root node has 0 keys, hacemos su primer hijo el nuevo root si tiene un hijo, caso  contrario ponemos el árbol como vacio
    if (root->n == 0) {
        BPlusNode<T> *tmp = root;
        if (root->leaf)
            root = nullptr; // The tree is empty now
        else
            root = root->C[0]; // The first child becomes the new root
        delete tmp;
    }
}

template <typename T>
int BPlusNode<T>::findKey(T k) { 
    int idx = 0;
    while (idx < n && keys[idx] < k)
        ++idx;
    return idx;
}

template <typename T>
void BPlusNode<T>::remove(T k) {
    int idx = findKey(k);
    if (idx < n && keys[idx] == k) {
        // If the node is a leaf node - removeFromLeaf is called
        //Sino removeFromNonLeaf is callled
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    } else {
        // If this node is a leaf node, entonces el key no esta en el b+ 
        if (leaf) {
            cout << "The key " << k << " is does not exist in the tree\n";
            return;
        }

// The key to be removed is present in the sub-tree rooted with this node, flag indicates if it's in the last child.
        bool flag = ((idx == n) ? true : false);
        if (C[idx]->n < t)
            fill(idx);
        if (flag && idx > n)
            C[idx - 1]->remove(k);
        else
            C[idx]->remove(k);
    }
    return;
}

template <typename T>
void BPlusNode<T>::removeFromLeaf(int idx) {
    // Move all other keys one step behind
    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    // Reduce the count of keys
    n--;

    return;
}


template <typename T>
void BPlusNode<T>::removeFromNonLeaf(int idx) {
    T k = keys[idx];

 // Si el hijo en la posición idx tiene al menos t claves, encuentra el predecesor de la clave y elimina recursivamente el predecesor
    if (C[idx]->n >= t) {
        T pred = getPred(idx);
        keys[idx] = pred;
        C[idx]->remove(pred);
    }

    // Si el hijo en la posición idx tiene menos de t claves, entonces encuentra el sucesor en el subárbol derecho y elimina recursivamente el sucesor
    else if (C[idx + 1]->n >= t) {
        T succ = getSucc(idx);
        keys[idx] = succ;
        C[idx + 1]->remove(succ);
    }

    // If both the child at idx and the child after idx has less than t keys,
    // merge them.
    else {
        merge(idx);
        C[idx]->remove(k);
    }
    return;
}

template <typename T>
T BPlusNode<T>::getPred(int idx) {
    // Keep moving to the right most node hasta llegar a una hoja
    BPlusNode<T> * cur = C[idx];
    while (!cur->leaf)
        cur = cur->C[cur->n];

    // Return the last key of the leaf
    return cur->keys[cur->n - 1];
}

template <typename T>
T BPlusNode<T>::getSucc(int idx) {
    // Keep moving the left most node starting from C[idx+1] hasta llegar a una hoja tmb ga
    BPlusNode<T>* cur = C[idx + 1];
    while (!cur->leaf)
        cur = cur->C[0];

    return cur->keys[0];
}

template <typename T>
void BPlusNode<T>::fill(int idx) {
    if (idx != 0 && C[idx - 1]->n >= t)
        borrowFromPrev(idx);
    else if (idx != n && C[idx + 1]->n >= t)
        borrowFromNext(idx);
    else {
        if (idx != n)
            merge(idx);
        else
            merge(idx - 1);
    }
    return;
}

template <typename T>
void BPlusNode<T>::borrowFromPrev(int idx) {
    BPlusNode<T>* child = C[idx]; // Hijo
    BPlusNode<T>* sibling = C[idx - 1]; // Hermano previo

    // Movemos todas las claves del hijo una posición adelante, para hacer espacio para la clave del nodo
    for (int i = child->n - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    // Mover hijos del hijo una posición adelante
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i)
            child->C[i + 1] = child->C[i];
    }

    // Copiamos la key del nodo al hijo
    child->keys[0] = keys[idx - 1];

    if (!leaf)
        child->C[0] = sibling->C[sibling->n];

    keys[idx - 1] = sibling->keys[sibling->n - 1];

    // Actualizar conteo del num de keys
    child->n += 1;
    sibling->n -= 1;

    return;
}

template <typename T>
void BPlusNode<T>::borrowFromNext(int idx) {
    // Lo mismo que borrowFromPrev, pero con el hermano siguiente
    BPlusNode<T>* child = C[idx];
    BPlusNode<T>* sibling = C[idx + 1];

    child->keys[(child->n)] = keys[idx];

    if (!(child->leaf))
        child->C[(child->n) + 1] = sibling->C[0]; 

    keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->C[i - 1] = sibling->C[i];
    }

    child->n += 1;
    sibling->n -= 1;

    return;
}

template <typename T>
void BPlusNode<T>::merge(int idx) { // Merge idx-th child of the node with (idx+1)-th child of the node
    BPlusNode<T>* child = C[idx];
    BPlusNode<T>* sibling = C[idx + 1];

    child->keys[t - 1] = keys[idx];

    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + t] = sibling->keys[i];

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->C[i + t] = sibling->C[i];
    }

    for (int i = idx + 1; i < n; ++i)
        keys[i - 1] = keys[i];

    for (int i = idx + 2; i <= n; ++i)
        C[i - 1] = C[i];

    child->n += sibling->n + 1;
    n--;

    delete(sibling);
    return;
}


