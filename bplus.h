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
void deleteFromNode(BPlusTree* tree, BPlusNode* node, int key);
void repairAfterDelete(BPlusTree* tree, BPlusNode* node);
void stealFromLeft(BPlusTree* tree, BPlusNode* node, int parentIndex);
void stealFromRight(BPlusTree* tree, BPlusNode* node, int parentIndex);
void mergeWithSibling(BPlusTree* tree, BPlusNode* left, BPlusNode* right, int parentIndex);


//teste
void printLeafNodes(BPlusTree* tree);

// Declarações das Funções

// Encontra o nó e índice da chave na árvore
BPlusNode* achaElemento(BPlusNode* noAtual, int* indice, int chave);

// Verifica o caso específico da remoção
int verificaCaso(BPlusNode* noAtual);

// Remove uma chave do nó e ajusta a árvore, se necessário
void remover(BPlusNode* noAtual, int chave);

// Ajusta a árvore após a remoção para manter suas propriedades
void arrumarArvore(BPlusNode* noAtual);

// Empresta uma chave do irmão à esquerda
void emprestaEsquerda(BPlusNode* noAtual);

// Empresta uma chave do irmão à direita
void emprestaDireita(BPlusNode* noAtual);

// Realiza o merge com o irmão à esquerda
void mergeEsquerda(BPlusNode* noAtual);

// Realiza o merge com o irmão à direita
void mergeDireita(BPlusNode* noAtual);

#endif // BPLUS_H
