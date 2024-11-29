#ifndef BPLUS_H
#define BPLUS_H

#include <stdio.h>
#include <stdlib.h>

// Estrutura do nó da árvore B+
typedef struct BPlusNode {
    int numKeys;                   // Número atual de chaves
    int keys[3];                   // Vetor de chaves (máximo 2 chaves para ordem 3 + 1 de overflow)
    struct BPlusNode* parent;      // Ponteiro para o nó pai
    struct BPlusNode* children[3]; // Ponteiros para os filhos (máximo 3 filhos para ordem 3)
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
BPlusNode* createNode();
BPlusTree* createTree();
BPlusNode* findLeaf(BPlusTree* tree, int key);

// Funções auxiliares

//teste
void printNode(BPlusNode* node);
void printNodeKeys(BPlusNode* node, int level);
void printTree(BPlusNode* root, int level);

#endif // BPLUS_H
