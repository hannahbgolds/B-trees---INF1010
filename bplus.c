#include "bplus.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
===========================================================
                FUNÇÕES DE SETUP
===========================================================
*/

// Função para criar um novo nó
BPlusNode* createNode(int isLeaf) {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    if (!node) {
        printf("Erro: Falha na alocação de memória para o nó.\n");
        return NULL;
    }

    node->numKeys = 0;
    node->parent = NULL;
    node->isLeaf = isLeaf;
    for (int i = 0; i < ORDER + 1; i++) {
        node->keys[i] = 0;         // Inicializa as chaves com 0
        node->children[i] = NULL;  // Inicializa os filhos com NULL
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}

// Função para criar uma nova árvore B+
BPlusTree* createTree(int order) {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    if (!tree) {
        printf("Erro: Falha na alocação de memória para a árvore.\n");
        return NULL;
    }

    tree->order = order;
    tree->root = createNode(1); // A raiz começa como folha
    printf("Árvore B+ criada com sucesso.\n");
    return tree;
}

/* 
===========================================================
                FUNÇÕES DE INSERÇÃO
===========================================================
*/

// Função para inserir uma chave na árvore
void insertKey(BPlusTree* tree, int key) {
    BPlusNode* current = tree->root;

    // Navega até a folha apropriada
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->numKeys && key > current->keys[i]) i++;
        current = current->children[i];
    }

    // Insere a chave em ordem no nó folha
    int i = current->numKeys - 1;
    while (i >= 0 && key < current->keys[i]) {
        current->keys[i + 1] = current->keys[i];
        i--;
    }

    if (i + 1 >= tree->order) {
        printf("Erro: Limite do array de chaves excedido durante a inserção.\n");
        return;
    }

    current->keys[i + 1] = key;
    current->numKeys++;

    printf("Chave %d inserida na folha.\n", key);

    // Divide o nó folha se transbordar
    if (current->numKeys == tree->order) {
        splitLeaf(tree, current);
    }
}

// Função para dividir um nó folha
void splitLeaf(BPlusTree* tree, BPlusNode* leaf) {
    int medianIndex = leaf->numKeys / 2;

    // Cria um novo nó folha
    BPlusNode* newLeaf = createNode(1);
    if (!newLeaf) {
        printf("Falha na alocação de memória para o novo nó folha.\n");
        return;
    }

    newLeaf->numKeys = leaf->numKeys - medianIndex;

    // Transfere chaves para o novo nó folha
    for (int i = 0; i < newLeaf->numKeys; i++) {
        newLeaf->keys[i] = leaf->keys[medianIndex + i];
    }
    leaf->numKeys = medianIndex;

    // Ajusta os ponteiros da lista ligada das folhas
    newLeaf->next = leaf->next;
    if (newLeaf->next) {
        newLeaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;

    printf("Nó folha dividido. Chave promovida: %d\n", newLeaf->keys[0]);

    // Promove a primeira chave do novo nó folha
    int promotedKey = newLeaf->keys[0];

    if (leaf->parent == NULL) {
        // Cria uma nova raiz se o nó folha não tiver pai
        BPlusNode* newRoot = createNode(0);
        if (!newRoot) {
            printf("Erro: Falha de alocação de memória para a nova raiz.\n");
            return;
        }

        newRoot->keys[0] = promotedKey;
        newRoot->children[0] = leaf;
        newRoot->children[1] = newLeaf;
        newRoot->numKeys = 1;

        leaf->parent = newRoot;
        newLeaf->parent = newRoot;
        tree->root = newRoot;

        printf("Nova raiz criada com a chave promovida: %d\n", promotedKey);
    } else {
        // Insere a chave promovida no nó interno
        insertInternal(tree, leaf->parent, promotedKey, newLeaf);
    }
}

// Função para inserir em um nó interno
void insertInternal(BPlusTree* tree, BPlusNode* parent, int key, BPlusNode* rightChild) {
    int i = parent->numKeys - 1;

    // Insere a chave no nó interno na posição correta
    while (i >= 0 && key < parent->keys[i]) {
        parent->keys[i + 1] = parent->keys[i];
        parent->children[i + 2] = parent->children[i + 1];
        i--;
    }
    parent->keys[i + 1] = key;
    parent->children[i + 2] = rightChild;
    parent->numKeys++;

    // Assegura que o filho direito tenha o pai correto
    rightChild->parent = parent;

    printf("Chave %d inserida no nó interno.\n", key);

    // Verifica se o nó interno transbordou
    if (parent->numKeys == tree->order) {
        splitInternal(tree, parent);
    }
}

// Função para dividir um nó interno
void splitInternal(BPlusTree* tree, BPlusNode* node) {
    int medianIndex = node->numKeys / 2;
    int promotedKey = node->keys[medianIndex];

    // Cria um novo nó interno
    BPlusNode* newInternal = createNode(0);
    if (!newInternal) {
        printf("Erro: Falha na alocação de memória para o novo nó interno.\n");
        return;
    }
    newInternal->numKeys = node->numKeys - medianIndex - 1;

    // Transfere chaves e filhos para o novo nó interno
    for (int i = 0; i < newInternal->numKeys; i++) {
        newInternal->keys[i] = node->keys[medianIndex + 1 + i];
        newInternal->children[i] = node->children[medianIndex + 1 + i];
        if (newInternal->children[i]) {
            newInternal->children[i]->parent = newInternal;
        }
    }
    newInternal->children[newInternal->numKeys] = node->children[node->numKeys];
    if (newInternal->children[newInternal->numKeys]) {
        newInternal->children[newInternal->numKeys]->parent = newInternal;
    }

    node->numKeys = medianIndex;

    printf("Nó interno dividido. Chave promovida: %d\n", promotedKey);

    if (node->parent == NULL) {
        // Cria uma nova raiz se o nó atual for a raiz
        BPlusNode* newRoot = createNode(0);
        if (!newRoot) {
            printf("Erro: Falha de alocação de memória para a nova raiz.\n");
            return;
        }

        newRoot->keys[0] = promotedKey;
        newRoot->children[0] = node;
        newRoot->children[1] = newInternal;
        newRoot->numKeys = 1;

        node->parent = newRoot;
        newInternal->parent = newRoot;
        tree->root = newRoot;

        printf("Nova raiz criada com a chave promovida: %d\n", promotedKey);
    } else {
        // Promove a chave para o nó pai
        insertInternal(tree, node->parent, promotedKey, newInternal);
    }
}

/* 
===========================================================
                FUNÇÕES DE BUSCA
===========================================================
*/

// Função para buscar uma chave na árvore
BPlusNode* searchKey(BPlusTree* tree, int key) {
    BPlusNode* current = tree->root;

    while (current != NULL) {
        int i = 0;

        while (i < current->numKeys && key > current->keys[i]) {
            i++;
        }

        if (i < current->numKeys && key == current->keys[i]) {
            printf("Chave %d encontrada no nó.\n", key);
            return current;
        }

        if (current->isLeaf) {
            break;
        }

        current = current->children[i];
    }

    printf("Chave %d não encontrada na árvore.\n", key);
    return NULL;
}

/* 
===========================================================
                FUNÇÕES DE IMPRESSÃO E DEBUG
===========================================================
*/

// Função auxiliar para imprimir os nós folhas (apenas para visualização)
void printLeafNodes(BPlusTree* tree) {
    BPlusNode* current = tree->root;
    while (!current->isLeaf) current = current->children[0];

    printf("Folhas encadeadas: ");
    while (current) {
        for (int i = 0; i < current->numKeys; i++) printf("%d ", current->keys[i]);
        printf("-> ");
        current = current->next;
    }
    printf("NULL\n");
}

// Função para imprimir a árvore B+
void printTree(BPlusTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("Árvore está vazia ou não inicializada.\n");
        return;
    }
    printNode(tree->root, 0);
}

// Função auxiliar para imprimir os nós da árvore
void printNode(BPlusNode* node, int level) {
    if (!node) return; // Verificação para evitar segmentação
    printf("Nível %d: [", level);
    for (int i = 0; i < node->numKeys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("]\n");

    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++) {
            if (node->children[i]) {
                printNode(node->children[i], level + 1);
            }
        }
    }
}

// Função para imprimir as chaves dos filhos de um nó
void printChildrenKeys(BPlusNode* node) {
    printf("Filhos do nó: ");
    for (int i = 0; i <= node->numKeys; i++) {
        if (node->children[i]) {
            printf("[ ");
            for (int j = 0; j < node->children[i]->numKeys; j++) {
                printf("%d ", node->children[i]->keys[j]);
            }
            printf("] ");
        } else {
            printf("[NULL] ");
        }
    }
    printf("\n");
}

// Função para depurar a árvore (impressão detalhada)
void debugTree(BPlusNode* node, int level) {
    if (!node) return;

    printf("Nível %d: [", level);
    for (int i = 0; i < node->numKeys; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("]\n");

    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++) {
            debugTree(node->children[i], level + 1);
        }
    }
}

/* 
===========================================================
                FUNÇÕES DE EXCLUSÃO
===========================================================
*/

// Função para encontrar o índice de uma chave em um nó
int findKeyIndex(BPlusNode* node, int key) {
    int idx = 0;
    while (idx < node->numKeys && node->keys[idx] < key) {
        idx++;
    }
    return idx;
}

// Função para remover uma chave de um nó
void removeKeyFromNode(BPlusNode* node, int key) {
    int idx = -1;
    for (int i = 0; i < node->numKeys; i++) {
        if (node->keys[i] == key) {
            idx = i;
            break;
        }
    }
    if (idx == -1) {
        // Chave não encontrada no nó
        printf("Chave %d não encontrada no nó.\n", key);
        return;
    }
    // Desloca as chaves e filhos (se for nó interno)
    for (int i = idx; i < node->numKeys - 1; i++) {
        node->keys[i] = node->keys[i + 1];
        if (!node->isLeaf) {
            node->children[i + 1] = node->children[i + 2];
        }
    }
    node->numKeys--;
    printf("Chave %d removida do nó.\n", key);
}

// Função para encontrar o nó folha que deve conter a chave
BPlusNode* findLeafNode(BPlusTree* tree, int key) {
    if (tree == NULL || tree->root == NULL) {
        printf("Árvore ou raiz não existe.\n");
        return NULL;
    }

    BPlusNode* current = tree->root;

    while (!current->isLeaf) {
        int i = 0;

        // Encontra o índice apropriado no nó atual
        while (i < current->numKeys && key >= current->keys[i]) {
            i++;
        }

        current = current->children[i];
    }

    // Verifica se a chave existe no nó folha
    for (int i = 0; i < current->numKeys; i++) {
        if (current->keys[i] == key) {
            printf("Nó folha contendo a chave %d encontrado.\n", key);
            return current;
        }
    }

    printf("Chave %d não encontrada nos nós folhas.\n", key);
    return NULL;
}

// Função para verificar se um nó tem o número mínimo de chaves
int nodeHasMinimumKeys(BPlusNode* node) {
    if (node->parent == NULL) {
        // O nó raiz pode ter menos chaves
        return 1;
    }
    int minKeys = (int)ceil((ORDER - 1) / 2.0);
    return node->numKeys >= minKeys;
}

// Função para obter um nó irmão para redistribuição ou fusão
BPlusNode* getSibling(BPlusNode* node, int* isLeftSibling) {
    BPlusNode* parent = node->parent;
    if (parent == NULL) {
        return NULL;
    }
    int idx = 0;
    while (idx <= parent->numKeys && parent->children[idx] != node) {
        idx++;
    }
    // Tenta o irmão esquerdo
    if (idx > 0) {
        *isLeftSibling = 1;
        return parent->children[idx - 1];
    }
    // Tenta o irmão direito
    if (idx < parent->numKeys) {
        *isLeftSibling = 0;
        return parent->children[idx + 1];
    }
    return NULL;
}

// Função para verificar se o irmão tem chaves extras para redistribuição
int siblingHasExtraKeys(BPlusNode* sibling) {
    int minKeys = (int)ceil((ORDER - 1) / 2.0);
    return sibling->numKeys > minKeys;
}

// Função para redistribuir chaves entre o nó e seu irmão
void redistributeKeys(BPlusNode* node, BPlusNode* sibling, BPlusNode* parent, int isLeftSibling, int idxInParent) {
    if (isLeftSibling) {
        // Move a última chave do irmão esquerdo para a primeira posição do nó
        for (int i = node->numKeys; i > 0; i--) {
            node->keys[i] = node->keys[i - 1];
            if (!node->isLeaf) {
                node->children[i + 1] = node->children[i];
            }
        }
        node->keys[0] = sibling->keys[sibling->numKeys - 1];
        if (!node->isLeaf) {
            node->children[1] = node->children[0];
            node->children[0] = sibling->children[sibling->numKeys];
            if (node->children[0]) {
                node->children[0]->parent = node;
            }
        }
        node->numKeys++;
        sibling->numKeys--;
        // Atualiza a chave no pai
        parent->keys[idxInParent - 1] = node->keys[0];
    } else {
        // Move a primeira chave do irmão direito para a última posição do nó
        node->keys[node->numKeys] = sibling->keys[0];
        if (!node->isLeaf) {
            node->children[node->numKeys + 1] = sibling->children[0];
            if (node->children[node->numKeys + 1]) {
                node->children[node->numKeys + 1]->parent = node;
            }
        }
        node->numKeys++;
        // Desloca as chaves e filhos no irmão
        for (int i = 0; i < sibling->numKeys - 1; i++) {
            sibling->keys[i] = sibling->keys[i + 1];
            if (!sibling->isLeaf) {
                sibling->children[i] = sibling->children[i + 1];
            }
        }
        if (!sibling->isLeaf) {
            sibling->children[sibling->numKeys - 1] = sibling->children[sibling->numKeys];
        }
        sibling->numKeys--;
        // Atualiza a chave no pai
        parent->keys[idxInParent] = sibling->keys[0];
    }
    printf("Redistribuição de chaves realizada entre nós.\n");
}

// Função para mesclar dois nós e ajustar o pai
void mergeNodes(BPlusNode* node, BPlusNode* sibling, BPlusNode* parent, int isLeftSibling, int idxInParent) {
    if (isLeftSibling) {
        // Mescla o nó atual no irmão esquerdo
        int startIdx = sibling->numKeys;

        // Para nós internos, traz a chave separadora do pai
        if (!node->isLeaf) {
            sibling->keys[startIdx] = parent->keys[idxInParent - 1];
            sibling->numKeys++;
            startIdx++;
        }

        // Copia chaves e filhos do nó para o irmão
        for (int i = 0; i < node->numKeys; i++) {
            sibling->keys[startIdx + i] = node->keys[i];
        }

        if (!node->isLeaf) {
            // Copia os ponteiros dos filhos
            for (int i = 0; i <= node->numKeys; i++) {
                sibling->children[startIdx + i] = node->children[i];
                if (sibling->children[startIdx + i]) {
                    sibling->children[startIdx + i]->parent = sibling;
                }
            }
        } else {
            // Ajusta os ponteiros dos nós folhas
            sibling->next = node->next;
            if (node->next) {
                node->next->prev = sibling;
            }
        }

        sibling->numKeys += node->numKeys;

        // Remove a chave e o ponteiro do filho do pai
        for (int i = idxInParent - 1; i < parent->numKeys - 1; i++) {
            parent->keys[i] = parent->keys[i + 1];
            parent->children[i + 1] = parent->children[i + 2];
        }
        parent->numKeys--;

        free(node);
    } else {
        // Mescla o irmão direito no nó atual
        int startIdx = node->numKeys;

        // Para nós internos, traz a chave separadora do pai
        if (!node->isLeaf) {
            node->keys[startIdx] = parent->keys[idxInParent];
            node->numKeys++;
            startIdx++;
        }

        // Copia chaves e filhos do irmão para o nó
        for (int i = 0; i < sibling->numKeys; i++) {
            node->keys[startIdx + i] = sibling->keys[i];
        }

        if (!sibling->isLeaf) {
            // Copia os ponteiros dos filhos
            for (int i = 0; i <= sibling->numKeys; i++) {
                node->children[startIdx + i] = sibling->children[i];
                if (node->children[startIdx + i]) {
                    node->children[startIdx + i]->parent = node;
                }
            }
        } else {
            // Ajusta os ponteiros dos nós folhas
            node->next = sibling->next;
            if (sibling->next) {
                sibling->next->prev = node;
            }
        }

        node->numKeys += sibling->numKeys;

        // Remove a chave e o ponteiro do filho do pai
        for (int i = idxInParent; i < parent->numKeys - 1; i++) {
            parent->keys[i] = parent->keys[i + 1];
            parent->children[i + 1] = parent->children[i + 2];
        }
        parent->numKeys--;

        free(sibling);
    }
    printf("Mesclagem de nós realizada.\n");
}

// Função para tratar o underflow em um nó após a exclusão
void handleUnderflow(BPlusNode* node, BPlusTree* tree) {
    if (node->parent == NULL) {
        // O nó é a raiz
        if (node->numKeys == 0 && !node->isLeaf) {
            // A raiz não tem chaves e tem um filho
            tree->root = node->children[0];
            tree->root->parent = NULL;
            free(node);
            printf("A raiz foi atualizada após underflow.\n");
        }
        return;
    }
    BPlusNode* parent = node->parent;
    int isLeftSibling;
    BPlusNode* sibling = getSibling(node, &isLeftSibling);
    int idxInParent = 0;
    while (idxInParent <= parent->numKeys && parent->children[idxInParent] != node) {
        idxInParent++;
    }
    if (sibling != NULL && siblingHasExtraKeys(sibling)) {
        // Redistribui chaves
        redistributeKeys(node, sibling, parent, isLeftSibling, idxInParent);
    } else {
        // Mescla os nós
        mergeNodes(node, sibling, parent, isLeftSibling, idxInParent);
        // Verifica se há underflow no pai
        if (parent->numKeys < (int)ceil((ORDER - 1) / 2.0) && parent->parent != NULL) {
            handleUnderflow(parent, tree);
        } else if (parent->numKeys == 0 && parent->parent == NULL) {
            // O pai é a raiz e não tem chaves
            tree->root = node;
            node->parent = NULL;
            free(parent);
            printf("A raiz foi atualizada após mesclagem.\n");
        }
    }
}

// Função para atualizar os nós internos após a exclusão de uma chave
void updateInternalKeysAfterDeletion(BPlusNode* node, int oldKey) {
    BPlusNode* current = node;
    int newKey = -1;

    // Se o nó for folha e ainda tiver chaves, o novo separador é a primeira chave
    if (current->numKeys > 0) {
        newKey = current->keys[0];
    } else {
        // Caso o nó folha esteja vazio, geralmente tratado pelo underflow
        return;
    }

    while (current->parent != NULL) {
        BPlusNode* parent = current->parent;
        int index = 0;

        // Encontra o índice do nó atual no array de filhos do pai
        while (index <= parent->numKeys && parent->children[index] != current) {
            index++;
        }

        if (index == 0) {
            // Nó atual é o filho mais à esquerda; não há chave separadora para atualizar
            current = parent;
            continue;
        }

        // Verifica se a chave separadora no pai é igual à chave excluída
        if (parent->keys[index - 1] == oldKey) {
            parent->keys[index - 1] = newKey;
        } else {
            // Não são necessárias mais atualizações
            break;
        }

        current = parent;
        // Atualiza newKey para o próximo nível
        newKey = parent->keys[0];
    }
    printf("Chaves internas atualizadas após exclusão.\n");
}

// Função para excluir uma chave da árvore B+
void deleteKey(BPlusTree* tree, int key) {
    BPlusNode* leafNode = findLeafNode(tree, key);
    if (leafNode == NULL) {
        // Chave não encontrada
        printf("Chave %d não encontrada para exclusão.\n", key);
        return;
    }
    removeKeyFromNode(leafNode, key);
    if (leafNode->parent == NULL || nodeHasMinimumKeys(leafNode)) {
        // Sem underflow ou nó raiz
        // Atualiza nós internos se necessário
        updateInternalKeysAfterDeletion(leafNode, key);
        return;
    }
    handleUnderflow(leafNode, tree);
}
