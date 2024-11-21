#ifndef BPLUS_H
#define BPLUS_H

#include <stdio.h>
#include <stdlib.h>

// Estrutura do nó da árvore B+
typedef struct BPlusNode {
    int numKeys;                   // Número atual de chaves
    int keys[3];                   // Vetor de chaves (máximo 2 chaves para ordem 3)
    struct BPlusNode* parent;      // Ponteiro para o nó pai
    struct BPlusNode* children[4]; // Ponteiros para os filhos (máximo 3 filhos para ordem 3)
    struct BPlusNode* next;        // Ponteiro para o próximo nó folha (apenas para folhas)
    struct BPlusNode* prev;        // Ponteiro para o nó folha anterior (apenas para folhas)
    int isLeaf;                    // 1 se for folha, 0 caso contrário
} BPlusNode;

// Estrutura da árvore B+
typedef struct BPlusTree {
    BPlusNode* root;               // Ponteiro para a raiz da árvore
    int order;                     // Ordem da árvore
} BPlusTree;

// Funções principais
BPlusTree* createTree(int order);
void insertKey(BPlusTree* tree, int key);
void deleteKey(BPlusTree* tree, int key);
void printTree(BPlusTree* tree);

// Funções auxiliares
BPlusNode* createNode(int isLeaf);
BPlusNode* searchKey(BPlusTree* tree, int key);
void splitLeaf(BPlusTree* tree, BPlusNode* leaf);
void splitInternal(BPlusTree* tree, BPlusNode* node);
void insertInternal(BPlusTree* tree, BPlusNode* parent, int key, BPlusNode* rightChild);
void handleUnderflow(BPlusTree* tree, BPlusNode* node);
void printNode(BPlusNode* node, int level);
void redistributeFromLeft(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* leftSibling, int nodeIndex);
void redistributeFromRight(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* rightSibling, int nodeIndex);
void mergeWithLeft(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* leftSibling, int nodeIndex);
void mergeWithRight(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* rightSibling, int nodeIndex);



//teste
void printLeafNodes(BPlusTree* tree);

#endif // BPLUS_H
