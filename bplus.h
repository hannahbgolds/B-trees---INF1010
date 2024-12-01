#ifndef BPLUS_H
#define BPLUS_H

#include <stdio.h>
#include <stdlib.h>

#define ORDER 3 // Define a ordem da árvore B+

/* 
===========================================================
                ESTRUTURAS DE DADOS
===========================================================
*/

// Estrutura do nó da árvore B+
typedef struct BPlusNode {
    int numKeys;                   // Número atual de chaves no nó
    int keys[3];                   // Vetor de chaves (máximo ORDER - 1 chaves)
    struct BPlusNode* parent;      // Ponteiro para o nó pai
    struct BPlusNode* children[4]; // Ponteiros para os filhos (máximo ORDER filhos)
    struct BPlusNode* next;        // Ponteiro para o próximo nó folha (apenas para folhas)
    struct BPlusNode* prev;        // Ponteiro para o nó folha anterior (apenas para folhas)
    int isLeaf;                    // 1 se for folha, 0 caso contrário
} BPlusNode;

// Estrutura da árvore B+
typedef struct BPlusTree {
    BPlusNode* root;               // Ponteiro para a raiz da árvore
    int order;                     // Ordem da árvore
} BPlusTree;

/* 
===========================================================
                PROTÓTIPOS DE FUNÇÕES
===========================================================
*/

/*** Funções de Setup ***/

// Cria um novo nó da árvore B+
BPlusNode* createNode(int isLeaf);

// Cria uma nova árvore B+
BPlusTree* createTree(int order);

/*** Funções de Inserção ***/

// Insere uma chave na árvore B+
void insertKey(BPlusTree* tree, int key);

// Divide um nó folha que transbordou
void splitLeaf(BPlusTree* tree, BPlusNode* leaf);

// Insere uma chave em um nó interno
void insertInternal(BPlusTree* tree, BPlusNode* parent, int key, BPlusNode* rightChild);

// Divide um nó interno que transbordou
void splitInternal(BPlusTree* tree, BPlusNode* node);

/*** Funções de Busca ***/

// Busca uma chave na árvore B+
BPlusNode* searchKey(BPlusTree* tree, int key);

/*** Funções de Impressão e Debug ***/

// Imprime as chaves dos filhos de um nó (para depuração)
void printChildrenKeys(BPlusNode* node);

// Imprime a árvore B+
void printTree(BPlusTree* tree);

// Função auxiliar para imprimir os nós da árvore
void printNode(BPlusNode* node, int level);

// Imprime os nós folhas da árvore (apenas para visualização)
void printLeafNodes(BPlusTree* tree);

// Função para depurar a árvore (impressão detalhada)
void debugTree(BPlusNode* node, int level);

/*** Funções de Exclusão ***/

// Encontra o índice de uma chave em um nó
int findKeyIndex(BPlusNode* node, int key);

// Remove uma chave de um nó
void removeKeyFromNode(BPlusNode* node, int key);

// Encontra o nó folha que deve conter a chave
BPlusNode* findLeafNode(BPlusTree* tree, int key);

// Verifica se um nó tem o número mínimo de chaves
int nodeHasMinimumKeys(BPlusNode* node);

// Obtém um nó irmão para redistribuição ou fusão
BPlusNode* getSibling(BPlusNode* node, int* isLeftSibling);

// Verifica se o irmão tem chaves extras para redistribuição
int siblingHasExtraKeys(BPlusNode* sibling);

// Redistribui chaves entre o nó e seu irmão
void redistributeKeys(BPlusNode* node, BPlusNode* sibling, BPlusNode* parent, int isLeftSibling, int idxInParent);

// Mescla dois nós e ajusta o pai
void mergeNodes(BPlusNode* node, BPlusNode* sibling, BPlusNode* parent, int isLeftSibling, int idxInParent);

// Trata o underflow em um nó após a exclusão
void handleUnderflow(BPlusNode* node, BPlusTree* tree);

// Atualiza os nós internos após a exclusão de uma chave
void updateInternalKeysAfterDeletion(BPlusNode* node, int oldKey);

// Exclui uma chave da árvore B+
void deleteKey(BPlusTree* tree, int key);

#endif // BPLUS_H
