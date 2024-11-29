#include "bplus.h"
#include <stdio.h>
#include <stdlib.h>

#define IS_LEAF 1
#define IS_ROOT 0

/////////////////////////////////////////////////////////////////////////
//                          Funções Principais                         //
/////////////////////////////////////////////////////////////////////////

BPlusNode* createNode() {
    BPlusNode* newNode = (BPlusNode*)malloc(sizeof(BPlusNode));
    if (newNode == NULL) {
        printf("Error: Failed to allocate memory for new node.");
        exit(1);
    }

    newNode->isLeaf = 0;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->parent = NULL;

    for (int i = 0; i < 3; i++) {
        newNode->children[i] = NULL;
        newNode->keys[i] = -1;
    }

    newNode->numKeys = 0;

    return newNode;
}

BPlusTree* createTree() {
    BPlusTree* newTree = (BPlusTree*)malloc(sizeof(BPlusTree));
    if (newTree == NULL) {
        printf("Error: Failed to allocate memory for new tree.");
        exit(1);
    }

    BPlusNode* root = createNode();
    root->isLeaf = IS_ROOT;

    newTree->order = 3;
    newTree->root = root;

    return newTree;
}

BPlusNode* findLeaf(BPlusTree* tree, int key) {
    
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
