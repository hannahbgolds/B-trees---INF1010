#include "bplus.h"
#include <stdio.h>
#include <stdlib.h>

// Criação de um novo nó
BPlusNode* createNode(int isLeaf) {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    if (!node) {
        printf("Error: Memory allocation failed for node.\n");
        return NULL;
    }

    node->numKeys = 0;
    node->parent = NULL;
    node->isLeaf = isLeaf;
    for (int i = 0; i < 4; i++) {
        node->keys[i] = 0;         // Initialize keys to 0
        node->children[i] = NULL; // Initialize children to NULL
    }
    node->next = NULL;
    node->prev = NULL;
    return node;
}


// Criação de uma nova árvore B+
BPlusTree* createTree(int order) {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->order = order;
    tree->root = createNode(1); // Raiz começa como folha
    return tree;
}

// Inserção de uma chave na árvore
void insertKey(BPlusTree* tree, int key) {
    BPlusNode* current = tree->root;

    // Navigate to the appropriate leaf
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->numKeys && key > current->keys[i]) i++;
        current = current->children[i];
    }

    // Insert the key in sorted order in the leaf
    int i = current->numKeys - 1;
    while (i >= 0 && key < current->keys[i]) {
        current->keys[i + 1] = current->keys[i];
        i--;
    }

    if (i + 1 >= tree->order) {
        printf("Error: Key array bounds exceeded during insertion.\n");
        return;
    }

    current->keys[i + 1] = key;
    current->numKeys++;

    // Split the leaf if it overflows
    if (current->numKeys == tree->order) {
        splitLeaf(tree, current);
    }
}


// Divisão de um nó folha
void splitLeaf(BPlusTree* tree, BPlusNode* leaf) {
    int medianIndex = leaf->numKeys / 2;

    printf("Before split, leaf keys: ");
    for (int i = 0; i < leaf->numKeys; i++) printf("%d ", leaf->keys[i]);
    printf("\n");

    // Create a new leaf
    BPlusNode* newLeaf = createNode(1);
    if (!newLeaf) {
        printf("Error: Memory allocation failed for new leaf.\n");
        return;
    }

    newLeaf->numKeys = leaf->numKeys - medianIndex;

    // Transfer keys to the new leaf
    for (int i = 0; i < newLeaf->numKeys; i++) {
        if (medianIndex + i < tree->order) {
            newLeaf->keys[i] = leaf->keys[medianIndex + i];
        } else {
            printf("Error: Median index exceeds bounds during split.\n");
            return;
        }
    }
    leaf->numKeys = medianIndex;

    printf("After split, original leaf keys: ");
    for (int i = 0; i < leaf->numKeys; i++) printf("%d ", leaf->keys[i]);
    printf("\n");

    printf("New leaf created with keys: ");
    for (int i = 0; i < newLeaf->numKeys; i++) printf("%d ", newLeaf->keys[i]);
    printf("\n");

    // Adjust linked list pointers
    newLeaf->next = leaf->next;
    if (newLeaf->next) {
        if (newLeaf->next->prev) {
            newLeaf->next->prev = newLeaf;
        } else {
            printf("Warning: next->prev is NULL during split.\n");
        }
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;

    // Promote the first key of the new leaf
    int promotedKey = newLeaf->keys[0];

    if (leaf->parent == NULL) {
        // Create a new root if the leaf has no parent
        BPlusNode* newRoot = createNode(0);
        if (!newRoot) {
            printf("Error: Memory allocation failed for new root.\n");
            return;
        }

        newRoot->keys[0] = promotedKey;
        newRoot->children[0] = leaf;
        newRoot->children[1] = newLeaf;
        newRoot->numKeys = 1;

        leaf->parent = newRoot;
        newLeaf->parent = newRoot;
        tree->root = newRoot;

        printf("New root created with promoted key: %d\n", promotedKey);
    } else {
        printf("Promoting key %d to parent node.\n", promotedKey);
        insertInternal(tree, leaf->parent, promotedKey, newLeaf);
    }
}



// Inserção em um nó interno
void insertInternal(BPlusTree* tree, BPlusNode* parent, int key, BPlusNode* rightChild) {
    int i = parent->numKeys - 1;

    // Inserir a chave no nó interno na posição correta
    while (i >= 0 && key < parent->keys[i]) {
        parent->keys[i + 1] = parent->keys[i];
        parent->children[i + 2] = parent->children[i + 1];
        i--;
    }
    parent->keys[i + 1] = key;
    parent->children[i + 2] = rightChild;
    parent->numKeys++;

    // Garantir que o filho direito seja associado corretamente
    rightChild->parent = parent;

    // Verificar se o nó interno transbordou
    if (parent->numKeys == tree->order) {
        splitInternal(tree, parent);
    }
}



// Divisão de um nó interno
void splitInternal(BPlusTree* tree, BPlusNode* node) {
    int medianIndex = node->numKeys / 2;
    int promotedKey = node->keys[medianIndex];

    printf("Dividindo nó interno com chaves: ");
    for (int i = 0; i < node->numKeys; i++) printf("%d ", node->keys[i]);
    printf("\n");

    BPlusNode* newInternal = createNode(0);
    newInternal->numKeys = node->numKeys - medianIndex - 1;

    // Transferir chaves e filhos para o novo nó interno
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

    if (node->parent == NULL) {
        // Criar nova raiz apenas se o nó atual for a raiz
        BPlusNode* newRoot = createNode(0);
        newRoot->keys[0] = promotedKey;
        newRoot->children[0] = node;
        newRoot->children[1] = newInternal;
        newRoot->numKeys = 1;

        node->parent = newRoot;
        newInternal->parent = newRoot;
        tree->root = newRoot;

        printf("Nova raiz criada com chave promovida: %d\n", promotedKey);
    } else {
        // Promover a chave para o nó pai
        insertInternal(tree, node->parent, promotedKey, newInternal);
    }
}

// Busca de uma chave na árvore
BPlusNode* searchKey(BPlusTree* tree, int key) {
    BPlusNode* current = tree->root;

    while (current != NULL) {
        int i = 0;

        while (i < current->numKeys && key > current->keys[i]) {
            i++;
        }

        if (i < current->numKeys && key == current->keys[i]) {
            return current;
        }

        if (current->isLeaf) {
            break;
        }

        current = current->children[i];
    }

    return NULL;
}

// Impressão da árvore B+
void printTree(BPlusTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("Árvore está vazia ou não inicializada.\n");
        return;
    }
    printNode(tree->root, 0);
}

void printNode(BPlusNode* node, int level) {
    if (!node) return; // Garantia contra segmentação
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


// Exclusão de uma chave da árvore
void deleteKey(BPlusTree* tree, int key) {
    // Buscar a folha contendo a chave
    BPlusNode* leaf = searchKey(tree, key);
    if (!leaf) {
        printf("Chave %d não encontrada.\n", key);
        return;
    }

    // Remover a chave da folha
    int i = 0;
    while (i < leaf->numKeys && leaf->keys[i] != key) i++;
    for (int j = i; j < leaf->numKeys - 1; j++) {
        leaf->keys[j] = leaf->keys[j + 1];
    }
    leaf->numKeys--;

    printf("Chave %d removida. Nó com %d chaves restantes: ", key, leaf->numKeys);
    for (int j = 0; j < leaf->numKeys; j++) {
        printf("%d ", leaf->keys[j]);
    }
    printf("\n");

    // Verificar underflow na folha
    if (leaf->numKeys < (tree->order + 1) / 2) {
        handleUnderflow(tree, leaf);
    }
}

void handleUnderflow(BPlusTree* tree, BPlusNode* node) {
    if (node->parent == NULL) {
        if (node->numKeys == 0 && !node->isLeaf) {
            printf("Raiz vazia após underflow. Reorganizando para o filho.\n");
            BPlusNode* newRoot = node->children[0];
            if (newRoot) {
                newRoot->parent = NULL;
            }
            tree->root = newRoot;
            free(node);
        }
        return;
    }

    BPlusNode* parent = node->parent;
    int nodeIndex = 0;

    while (nodeIndex <= parent->numKeys && parent->children[nodeIndex] != node) {
        nodeIndex++;
    }

    printf("Underflow detectado no nó com pai: %p\n", (void*)parent);

    BPlusNode* leftSibling = (nodeIndex > 0) ? parent->children[nodeIndex - 1] : NULL;
    BPlusNode* rightSibling = (nodeIndex < parent->numKeys) ? parent->children[nodeIndex + 1] : NULL;

    // Redistribuição ou Mesclagem
    if (leftSibling && leftSibling->numKeys > (tree->order - 1) / 2) {
        printf("Redistribuindo do irmão esquerdo.\n");
        redistributeFromLeft(tree, parent, node, leftSibling, nodeIndex);
    } else if (rightSibling && rightSibling->numKeys > (tree->order - 1) / 2) {
        printf("Redistribuindo do irmão direito.\n");
        redistributeFromRight(tree, parent, node, rightSibling, nodeIndex);
    } else if (leftSibling) {
        printf("Mesclando nó com irmão esquerdo.\n");
        mergeWithLeft(tree, parent, node, leftSibling, nodeIndex);
    } else if (rightSibling) {
        printf("Mesclando nó com irmão direito.\n");
        mergeWithRight(tree, parent, node, rightSibling, nodeIndex);
    }

    // Verificar se o pai deve ser reorganizado
    if (parent->numKeys == 0) {
        if (parent == tree->root) {
            printf("Reorganizando a raiz após mesclagem.\n");
            tree->root = parent->children[0];
            if (tree->root) {
                tree->root->parent = NULL;
            }
            free(parent);
        } else {
            printf("Propagando underflow para o pai.\n");
            handleUnderflow(tree, parent);
        }
    }
}



void mergeWithLeft(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* leftSibling, int nodeIndex) {
    printf("Mesclando nó com irmão esquerdo.\n");

    // Adicionar chave do pai ao irmão esquerdo
    leftSibling->keys[leftSibling->numKeys] = parent->keys[nodeIndex - 1];
    leftSibling->numKeys++;

    // Transferir chaves do nó para o irmão esquerdo
    for (int i = 0; i < node->numKeys; i++) {
        leftSibling->keys[leftSibling->numKeys] = node->keys[i];
        leftSibling->numKeys++;
    }

    // Transferir filhos se não for folha
    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++) {
            leftSibling->children[leftSibling->numKeys + i] = node->children[i];
            if (node->children[i]) {
                node->children[i]->parent = leftSibling;
            }
        }
    } else {
        leftSibling->next = node->next;
        if (node->next) {
            node->next->prev = leftSibling;
        }
    }

    // Atualizar pai
    for (int i = nodeIndex - 1; i < parent->numKeys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->numKeys--;

    free(node);
}

void mergeWithRight(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* rightSibling, int nodeIndex) {
    printf("Mesclando nó com irmão direito.\n");

    // Adicionar chave do pai ao nó
    node->keys[node->numKeys] = parent->keys[nodeIndex];
    node->numKeys++;

    // Transferir chaves do irmão direito para o nó
    for (int i = 0; i < rightSibling->numKeys; i++) {
        node->keys[node->numKeys] = rightSibling->keys[i];
        node->numKeys++;
    }

    // Transferir filhos se não for folha
    if (!node->isLeaf) {
        for (int i = 0; i <= rightSibling->numKeys; i++) {
            node->children[node->numKeys + i] = rightSibling->children[i];
            if (rightSibling->children[i]) {
                rightSibling->children[i]->parent = node;
            }
        }
    } else {
        node->next = rightSibling->next;
        if (rightSibling->next) {
            rightSibling->next->prev = node;
        }
    }

    // Atualizar pai
    for (int i = nodeIndex; i < parent->numKeys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->numKeys--;

    free(rightSibling);
}



// Função para redistribuir do irmão esquerdo
void redistributeFromLeft(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* leftSibling, int nodeIndex) {
    printf("Redistribuindo do irmão esquerdo: Nó atual tem %d chaves, irmão esquerdo tem %d chaves.\n", node->numKeys, leftSibling->numKeys);

    // Adicionar a chave do pai ao início do nó atual
    for (int i = node->numKeys; i > 0; i--) {
        node->keys[i] = node->keys[i - 1];
    }
    node->keys[0] = parent->keys[nodeIndex - 1];

    // Atualizar o pai com a nova chave do irmão esquerdo
    parent->keys[nodeIndex - 1] = leftSibling->keys[leftSibling->numKeys - 1];
    leftSibling->numKeys--;
    node->numKeys++;

    // Redistribuir filhos se não for folha
    if (!node->isLeaf) {
        for (int i = node->numKeys; i > 0; i--) {
            node->children[i] = node->children[i - 1];
        }
        node->children[0] = leftSibling->children[leftSibling->numKeys + 1];
        if (node->children[0]) {
            node->children[0]->parent = node;
        }
    }

    printf("Redistribuição do irmão esquerdo concluída: Nó atual tem %d chaves, irmão esquerdo tem %d chaves restantes.\n", node->numKeys, leftSibling->numKeys);
}


// Função para redistribuir do irmão direito
void redistributeFromRight(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* rightSibling, int nodeIndex) {
    printf("Redistribuindo do irmão direito: Nó atual tem %d chaves, irmão direito tem %d chaves.\n", node->numKeys, rightSibling->numKeys);

    // Adicionar a chave do pai ao nó atual
    node->keys[node->numKeys] = parent->keys[nodeIndex];
    node->numKeys++;

    // Atualizar o pai com a primeira chave do irmão direito
    parent->keys[nodeIndex] = rightSibling->keys[0];

    // Remover a chave promovida do irmão direito
    for (int i = 0; i < rightSibling->numKeys - 1; i++) {
        rightSibling->keys[i] = rightSibling->keys[i + 1];
    }
    rightSibling->numKeys--;

    // Ajustar filhos, se o nó não for folha
    if (!node->isLeaf) {
        // Mover o primeiro filho do irmão direito para o nó atual
        node->children[node->numKeys] = rightSibling->children[0];

        // Ajustar os filhos do irmão direito
        for (int i = 0; i < rightSibling->numKeys + 1; i++) {
            rightSibling->children[i] = rightSibling->children[i + 1];
        }

        // Atualizar o pai do filho movido
        if (node->children[node->numKeys]) {
            node->children[node->numKeys]->parent = node;
        }
    }
}




// Função para mesclar com o irmão esquerdo

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
