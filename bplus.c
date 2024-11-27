#include "bplus.h"
#include <stdio.h>
#include <stdlib.h>


void debugTree(BPlusNode* node, int level) {
    if (!node) return;

    printf("Level %d: [", level);
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

void validateRelationships(BPlusNode* node) {
    if (!node) return;

    for (int i = 0; i <= node->numKeys; i++) {
        if (node->children[i]) {
            if (node->children[i]->parent != node) {
                printf("Error: Parent-child relationship is inconsistent.\n");
                printf("Node keys: ");
                for (int j = 0; j < node->numKeys; j++) {
                    printf("%d ", node->keys[j]);
                }
                printf("\n");

                printf("Child keys: ");
                for (int j = 0; j < node->children[i]->numKeys; j++) {
                    printf("%d ", node->children[i]->keys[j]);
                }
                printf("\n");
            }
            validateRelationships(node->children[i]);
        }
    }
}


// SETUP
/////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////



//INSERÇÃO
////////////////////////////////////////////////////////////////
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

    
    // for (int i = 0; i < leaf->numKeys; i++) printf("%d ", leaf->keys[i]);
    // printf("\n");

    // Create a new leaf
    BPlusNode* newLeaf = createNode(1);
    if (!newLeaf) {
        printf("Falha na alocação de memória.\n");
        return;
    }

    newLeaf->numKeys = leaf->numKeys - medianIndex;

    // Transfer keys to the new leaf
    for (int i = 0; i < newLeaf->numKeys; i++) {
        if (medianIndex + i < tree->order) {
            newLeaf->keys[i] = leaf->keys[medianIndex + i];
        } else {
            printf("Erro.\n");
            return;
        }
    }
    leaf->numKeys = medianIndex;

    // printf("After split, original leaf keys: ");
    // for (int i = 0; i < leaf->numKeys; i++) printf("%d ", leaf->keys[i]);
    // printf("\n");

    // printf("New leaf created with keys: ");
    // for (int i = 0; i < newLeaf->numKeys; i++) printf("%d ", newLeaf->keys[i]);
    // printf("\n");

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
            printf("Erro: Falha de alocação de memória para nova raiz\n");
            return;
        }

        newRoot->keys[0] = promotedKey;
        newRoot->children[0] = leaf;
        newRoot->children[1] = newLeaf;
        newRoot->numKeys = 1;

        leaf->parent = newRoot;
        newLeaf->parent = newRoot;
        tree->root = newRoot;

        // printf("New root created with promoted key: %d\n", promotedKey);
    } else {
        // printf("Promoting key %d to parent node.\n", promotedKey);
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

    // printf("Dividindo nó interno com chaves: ");
    // for (int i = 0; i < node->numKeys; i++) printf("%d ", node->keys[i]);
    // printf("\n");

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

        // printf("Nova raiz criada com chave promovida: %d\n", promotedKey);
    } else {
        // Promover a chave para o nó pai
        insertInternal(tree, node->parent, promotedKey, newInternal);
    }
}
//////////////////////////////////////////////////////////////////////////////////////


//FUNÇÃO AUXILIAR PARA PRINTAR OS NÓS (APENAS PARA VISUALIZAÇÃO)
///////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////


//BUSCA
//////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////



//EXCLUSÃO

// int deleteKey (BPlusTree*tree, int key){
    //se eu retirar um dado que está só na folha: 
        //checar se a folha tem underflow (menos de m/2 chaves ocupando a folha após a remoção): 
            //se não tiver: 
                //só remove
            //se tiver: 
                //verificar se o número de chaves ocupando aquela folha + o número de chaves do seu irmão a direita|esquerda é menor que m:
                    // se um deles for: 
                        //concatena (se for para a direita: vai ficar as chaves da folha adjacente + a maior chave do pai + as chaves do defasado E
                        //a maior chave do pai, sai do pai)
            
    //if isLeaf = 1 (ta na folha), 
// }

// int mergeWithRight()

void deleteKey(BPlusTree* tree, int key) {
    if (tree->root == NULL) {
        printf("Error: Cannot delete from an empty tree.\n");
        return;
    }

    BPlusNode* leaf = searchKey(tree, key);

    if (leaf == NULL) {
        printf("Error: Key %d not found in the tree.\n", key);
        return;
    }

    printf("Deleting key %d...\n", key);

    // Call helper function to delete the key
    deleteFromNode(tree, leaf, key);

    // Check if root needs adjustment
    if (tree->root->numKeys == 0) {
        if (!tree->root->isLeaf) {
            // If the root is empty but has children, promote the first child as the new root
            BPlusNode* newRoot = tree->root->children[0];
            newRoot->parent = NULL;
            free(tree->root);
            tree->root = newRoot;
        } else {
            // If the root is a leaf and becomes empty, set the tree as empty
            free(tree->root);
            tree->root = NULL;
        }
    }
}

void deleteFromNode(BPlusTree* tree, BPlusNode* node, int key) {
    int i = 0;

    // Locate the key to delete
    while (i < node->numKeys && node->keys[i] != key) i++;

    if (i == node->numKeys) {
        printf("Error: Key %d not found in the node.\n", key);
        return;
    }

    // Shift keys to remove the deleted key
    for (int j = i; j < node->numKeys - 1; j++) {
        node->keys[j] = node->keys[j + 1];
    }
    node->numKeys--;

    if (node->isLeaf) {
        // If it's a leaf node, check for underflow
        if (node->numKeys < (tree->order - 1) / 2) {
            repairAfterDelete(tree, node);
        }
    } else {
        // Internal node: Replace with in-order successor
        BPlusNode* successor = node->children[i + 1];
        while (!successor->isLeaf) {
            successor = successor->children[0];
        }
        node->keys[i] = successor->keys[0];
        deleteFromNode(tree, successor, successor->keys[0]);
    }
}

void repairAfterDelete(BPlusTree* tree, BPlusNode* node) {
    if (node->parent == NULL) {
        // If the node is the root, no further repair needed
        printf("Node is root, no repair needed.\n");

        // If the root has no keys and a single child, promote the child to root
        if (tree->root->numKeys == 0 && !tree->root->isLeaf) {
            printf("Root has no keys. Promoting its only child as the new root.\n");
            BPlusNode* newRoot = tree->root->children[0];
            tree->root = newRoot;
            tree->root->parent = NULL;
        }

        return;
    }

    BPlusNode* parent = node->parent;
    int parentIndex = 0;

    // Find the index of the node in its parent's children array
    while (parentIndex <= parent->numKeys && parent->children[parentIndex] != node) {
        parentIndex++;
    }

    if (parentIndex > parent->numKeys) {
        printf("Error: Node not found in parent's children.\n");
        return;
    }

    printf("Parent keys before repair: ");
    for (int i = 0; i < parent->numKeys; i++) {
        printf("%d ", parent->keys[i]);
    }
    printf("\n");

    // Check if we can borrow or need to merge
    if (parentIndex > 0 && parent->children[parentIndex - 1]->numKeys > (tree->order - 1) / 2) {
        printf("Stealing from left sibling.\n");
        stealFromLeft(tree, node, parentIndex);
    } else if (parentIndex < parent->numKeys && parent->children[parentIndex + 1]->numKeys > (tree->order - 1) / 2) {
        printf("Stealing from right sibling.\n");
        stealFromRight(tree, node, parentIndex);
    } else if (parentIndex > 0) {
        printf("Merging with left sibling.\n");
        mergeWithSibling(tree, parent->children[parentIndex - 1], node, parentIndex - 1);
    } else {
        printf("Merging with right sibling.\n");
        mergeWithSibling(tree, node, parent->children[parentIndex + 1], parentIndex);
    }

    // Handle empty parent
    if (parent->numKeys == 0) {
        if (parent == tree->root) {
            printf("Parent is root and became empty. Promoting child as new root.\n");
            // Promote the child to the root
            if (parent->children[0]) {
                tree->root = parent->children[0];
                tree->root->parent = NULL;
            }
            free(parent);  // Free the old parent node
        } else {
            printf("Parent became empty. Recursively repairing parent.\n");

            // Promote the valid child to the parent's position
            for (int i = 0; i <= parent->numKeys; i++) {
                if (parent->children[i]) {
                    parent->children[i]->parent = parent->parent;
                }
            }

            BPlusNode* grandparent = parent->parent;

            // Free parent and repair grandparent safely
            free(parent);
            repairAfterDelete(tree, grandparent);  // Recursively repair grandparent
        }
    } else {
        // Ensure parent keys reflect the current state of its children
        if (parentIndex > 0 && parent->children[parentIndex - 1] != NULL) {
            parent->keys[parentIndex - 1] = parent->children[parentIndex]->keys[0];
        } else if (parentIndex < parent->numKeys && parent->children[parentIndex + 1] != NULL) {
            parent->keys[parentIndex] = parent->children[parentIndex + 1]->keys[0];
        }
    }

    // After repairs, forcefully update the parent and its children
    printf("Parent keys after repair: ");
    for (int i = 0; i < parent->numKeys; i++) {
        printf("%d ", parent->keys[i]);
    }
    printf("\n");
}


void stealFromLeft(BPlusTree* tree, BPlusNode* node, int parentIndex) {
    BPlusNode* leftSibling = node->parent->children[parentIndex - 1];

    // Shift keys in the current node to make space
    for (int i = node->numKeys; i > 0; i--) {
        node->keys[i] = node->keys[i - 1];
    }

    // Borrow the last key from the left sibling
    node->keys[0] = node->parent->keys[parentIndex - 1];
    node->parent->keys[parentIndex - 1] = leftSibling->keys[leftSibling->numKeys - 1];

    if (!node->isLeaf) {
        // Shift children pointers in the current node
        for (int i = node->numKeys + 1; i > 0; i--) {
            node->children[i] = node->children[i - 1];
        }

        // Borrow the last child from the left sibling
        node->children[0] = leftSibling->children[leftSibling->numKeys];
        if (node->children[0]) {
            node->children[0]->parent = node;
        }
    }

    node->numKeys++;
    leftSibling->numKeys--;
}

void stealFromRight(BPlusTree* tree, BPlusNode* node, int parentIndex) {
    BPlusNode* rightSibling = node->parent->children[parentIndex + 1];
    BPlusNode* parent = node->parent;

    // Move parent's separating key into the current node
    node->keys[node->numKeys] = parent->keys[parentIndex];
    node->numKeys++;

    // Update parent's separating key to the smallest key in the right sibling
    parent->keys[parentIndex] = rightSibling->keys[0];

    // Shift keys in the right sibling
    for (int i = 0; i < rightSibling->numKeys - 1; i++) {
        rightSibling->keys[i] = rightSibling->keys[i + 1];
    }

    // If nodes are not leaves, adjust child pointers
    if (!node->isLeaf) {
        node->children[node->numKeys] = rightSibling->children[0];
        if (node->children[node->numKeys]) {
            node->children[node->numKeys]->parent = node;
        }

        for (int i = 0; i < rightSibling->numKeys; i++) {
            rightSibling->children[i] = rightSibling->children[i + 1];
        }
    }

    // Decrease the key count in the right sibling
    rightSibling->numKeys--;
}


void mergeWithSibling(BPlusTree* tree, BPlusNode* leftNode, BPlusNode* rightNode, int parentIndex) {
    BPlusNode* parent = leftNode->parent;

    printf("\n--- STARTING MERGE ---\n");
    printf("Merging nodes with keys: ");
    for (int i = 0; i < leftNode->numKeys; i++) {
        printf("%d ", leftNode->keys[i]);
    }
    printf("and ");
    for (int i = 0; i < rightNode->numKeys; i++) {
        printf("%d ", rightNode->keys[i]);
    }
    printf("\n");

    // Always transfer the separating key from the parent to the left node
    printf("Adding separating key %d from parent to left node\n", parent->keys[parentIndex]);
    leftNode->keys[leftNode->numKeys++] = parent->keys[parentIndex];

    // Transfer keys from the right node to the left node
    for (int i = 0; i < rightNode->numKeys; i++) {
        leftNode->keys[leftNode->numKeys++] = rightNode->keys[i];
    }

    // Transfer children from right node to left node if it's not a leaf
    if (!leftNode->isLeaf) {
        for (int i = 0; i <= rightNode->numKeys; i++) {
            leftNode->children[leftNode->numKeys + i] = rightNode->children[i];
            if (rightNode->children[i]) {
                rightNode->children[i]->parent = leftNode;
            }
        }
    }

    // Now, remove duplicates from the left node (after merge)
    for (int i = 0; i < leftNode->numKeys - 1; i++) {
        if (leftNode->keys[i] == leftNode->keys[i + 1]) {
            // Shift left the duplicate key
            for (int j = i; j < leftNode->numKeys - 1; j++) {
                leftNode->keys[j] = leftNode->keys[j + 1];
            }
            leftNode->numKeys--;  // Decrease the number of keys in the left node
            i--; // Recheck the current index after shifting
        }
    }

    // Shift keys and children in the parent node
    for (int i = parentIndex; i < parent->numKeys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }

    // Clean up orphaned child reference
    parent->children[parent->numKeys] = NULL;
    parent->numKeys--;

    // If the parent becomes empty, we need to check if we should update the root
    if (parent->numKeys == 0 && parent == tree->root) {
        printf("Parent became empty. Updating root...\n");
        tree->root = leftNode;  // Make leftNode the new root
        tree->root->parent = NULL;  // Root has no parent
        free(parent);  // Free the old parent node
    }

    // Validate tree structure after merge
    printf("Validating tree structure after merge...\n");
    debugTree(tree->root, 0);

    // Validate parent-child relationships after merge
    printf("Validating parent-child relationships...\n");
    for (int i = 0; i <= parent->numKeys; i++) {
        if (parent->children[i] && parent->children[i]->parent != parent) {
            printf("Error: Parent-child relationship is inconsistent.\n");
        }
    }

    printf("--- END OF MERGE ---\n");
}
