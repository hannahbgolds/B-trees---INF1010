#include "bplus.h"
#include <stdio.h>
#include <stdlib.h>

#define ORDER 3
#define MAX_KEYS (ORDER - 1) 

/////////////////////////////////////////////////////////////////////////
//                          Funções Principais                         //
/////////////////////////////////////////////////////////////////////////

BPlusNode* createLeafNode() {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->numKeys = 0;
    node->isLeaf = 1;
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Create a new internal node
BPlusNode* createInternalNode() {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->numKeys = 0;
    node->isLeaf = 0;
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// Insert a key into the B+ Tree
void insertKey(BPlusTree* tree, int key) {

    printf("Inserting key %d...", key);

    if (!tree) {
        printf("Error: Tree is NULL\n");
        return;
    }

    // If the tree is empty, create a new root node and insert the key
    if (!tree->root) {
        BPlusNode* newRoot = createLeafNode();
        newRoot->keys[0] = key;
        newRoot->numKeys = 1;
        tree->root = newRoot;
        return;
    }

    // Find the leaf node where the key should be inserted
    BPlusNode* leaf = findLeafToInsert(tree, key);

    // Insert the key into the leaf node
    insertIntoLeaf(leaf, key);

    // If the leaf node overflows, split it
    if (leaf->numKeys > MAX_KEYS) {
        printf("Leaf has overflow, splitting...");
        splitLeafNode(tree, leaf);
    }
}

// Find the leaf node where the key should be inserted
BPlusNode* findLeafToInsert(BPlusTree* tree, int key) {
    BPlusNode* current = tree->root;

    while (!current->isLeaf) {
        int i = 0;
        while (i < current->numKeys && key >= current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }

    return current;
}

// Insert a key into a leaf node
void insertIntoLeaf(BPlusNode* leaf, int key) {
    int i = leaf->numKeys - 1;

    // Shift keys to make room for the new key
    while (i >= 0 && leaf->keys[i] > key) {
        leaf->keys[i + 1] = leaf->keys[i];
        i--;
    }

    // Insert the new key
    leaf->keys[i + 1] = key;
    leaf->numKeys++;
}

// Split a leaf node that has overflowed
void splitLeafNode(BPlusTree* tree, BPlusNode* leaf) {
    int tempKeys[ORDER];
    int i;

    // Copy keys to a temporary array
    for (i = 0; i < leaf->numKeys; i++) {
        tempKeys[i] = leaf->keys[i];
    }

    // Create a new leaf node
    BPlusNode* newLeaf = createLeafNode();

    // Determine the split point
    int splitPoint = (ORDER + 1) / 2;

    // Reset leaf node keys
    leaf->numKeys = splitPoint;
    newLeaf->numKeys = (ORDER) - splitPoint;

    // Copy keys to the leaf nodes
    for (i = 0; i < leaf->numKeys; i++) {
        leaf->keys[i] = tempKeys[i];
    }
    for (i = 0; i < newLeaf->numKeys; i++) {
        newLeaf->keys[i] = tempKeys[splitPoint + i];
    }

    // Update the linked list pointers
    newLeaf->next = leaf->next;
    if (newLeaf->next) {
        newLeaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;

    // Set parent pointers
    newLeaf->parent = leaf->parent;

    // Insert the first key of the new leaf into the parent node
    int newKey = newLeaf->keys[0];
    insertIntoParent(tree, leaf, newKey, newLeaf);
}

// Insert a key and child pointer into an internal node
void insertIntoParent(BPlusTree* tree, BPlusNode* left, int key, BPlusNode* right) {
    if (left->parent == NULL) {
        // Create a new root node
        BPlusNode* newRoot = createInternalNode();
        newRoot->keys[0] = key;
        newRoot->children[0] = left;
        newRoot->children[1] = right;
        newRoot->numKeys = 1;
        left->parent = newRoot;
        right->parent = newRoot;
        tree->root = newRoot;
        return;
    }

    BPlusNode* parent = left->parent;
    int i = parent->numKeys - 1;

    // Find the position to insert the new key
    while (i >= 0 && parent->keys[i] > key) {
        parent->keys[i + 1] = parent->keys[i];
        parent->children[i + 2] = parent->children[i + 1];
        i--;
    }

    // Insert the new key and child
    parent->keys[i + 1] = key;
    parent->children[i + 2] = right;
    parent->numKeys++;
    right->parent = parent;

    // If the parent overflows, split it
    if (parent->numKeys > MAX_KEYS) {
        splitInternalNode(tree, parent);
    }
}

// Split an internal node that has overflowed
void splitInternalNode(BPlusTree* tree, BPlusNode* node) {
    int tempKeys[ORDER];
    BPlusNode* tempChildren[ORDER + 1];
    int i, j;

    // Copy keys and children to temporary arrays
    for (i = 0; i < node->numKeys; i++) {
        tempKeys[i] = node->keys[i];
    }
    for (i = 0; i <= node->numKeys; i++) {
        tempChildren[i] = node->children[i];
    }

    // Create a new internal node
    BPlusNode* newNode = createInternalNode();

    // Determine the split point
    int splitPoint = (ORDER + 1) / 2;

    // Reset node keys and children
    node->numKeys = splitPoint;
    newNode->numKeys = (ORDER - 1) - splitPoint;

    // Copy keys and children to the original node
    for (i = 0; i < node->numKeys; i++) {
        node->keys[i] = tempKeys[i];
    }
    for (i = 0; i <= node->numKeys; i++) {
        node->children[i] = tempChildren[i];
        node->children[i]->parent = node;
    }

    // Copy keys and children to the new node
    for (i = 0, j = splitPoint + 1; j < ORDER; i++, j++) {
        newNode->keys[i] = tempKeys[j];
    }
    for (i = 0, j = splitPoint + 1; j <= ORDER; i++, j++) {
        newNode->children[i] = tempChildren[j];
        newNode->children[i]->parent = newNode;
    }

    // Set parent pointer
    newNode->parent = node->parent;

    // Promote the middle key to the parent
    int upKey = tempKeys[splitPoint];
    insertIntoParent(tree, node, upKey, newNode);
}



/////////////////////////////////////////////////////////////////////////
//                        Funções Auxiliares                           //
/////////////////////////////////////////////////////////////////////////

int checkTreeAndRoot(BPlusTree* tree) {
    if (tree == NULL) {
        printf("Error: Tree is NULL\n");
        return 1;
    }

    if (tree->root == NULL) {
        printf("Error: Root is NULL\n");
        return 1;
    }

    return 0;
}

BPlusNode* findLeafToInsert(BPlusTree* tree, int key) {

    if (checkTreeAndRoot(tree)) {
        printf("Error: Tree or Root is NULL\n");
        return NULL;
    }

    BPlusNode* current = tree->root;

    // Traverse the tree to find the appropriate leaf
    while (!current->isLeaf) {
        int i = 0;

        // Find the appropriate child pointer to follow
        while (i < current->numKeys && key >= current->keys[i]) {
            i++;
        }

        current = current->children[i];
    }

    return current; // Return the leaf where the key should be inserted
}



/////////////////////////////////////////////////////////////////////////
//                        Funções de Impressão                         //
/////////////////////////////////////////////////////////////////////////

void printNode(BPlusNode* node) {
    if (!node) {
        printf("Node: NULL\n");
        return;
    }

    printf("Node Address: %p\n", (void*)node);
    printf("Number of Keys: %d\n", node->numKeys);
    printf("Keys: ");
    for (int i = 0; i < node->numKeys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("\n");

    printf("Parent Address: %p\n", (void*)node->parent);

    if (node->isLeaf) {
        printf("Node Type: Leaf\n");
        printf("Next Leaf Address: %p\n", (void*)node->next);
        printf("Previous Leaf Address: %p\n", (void*)node->prev);
    } else {
        printf("Node Type: Internal\n");
        printf("Children Addresses: ");
        for (int i = 0; i <= node->numKeys; i++) {
            printf("%p ", (void*)node->children[i]);
        }
        printf("\n");
    }

    printf("------------------------------------\n");
}

#include <stdio.h>

void printNodeKeys(BPlusNode* node, int level) {
    if (!node) {
        printf("Nível %d: [NULL]\n", level);
        return;
    }

    printf("Nível %d: [", level);
    for (int i = 0; i < node->numKeys; i++) {
        printf("%d", node->keys[i]);
        if (i < node->numKeys - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

void printTree(BPlusNode* root, int level) {
    if (!root) {
        printf("Nível %d: [NULL]\n", level);
        return;
    }

    // Imprime as chaves do nó atual
    printNodeKeys(root, level);

    // Se não for folha, percorre os filhos
    if (!root->isLeaf) {
        for (int i = 0; i <= root->numKeys; i++) {
            printTree(root->children[i], level + 1);
        }
    }
}
