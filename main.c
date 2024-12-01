#include "bplus.h"

int main() {
    BPlusTree* tree = createTree(3);

    // Inserir chaves
    printf("Inserindo chaves: 15, 5, 25\n");
    insertKey(tree, 10);
    insertKey(tree, 15);
    insertKey(tree, 20);
    insertKey(tree, 5);
    insertKey(tree, 25);
    insertKey(tree, 30);
    insertKey(tree, 35);
   

    // Imprimir a árvore após inserções
    printf("\nÁrvore após inserções:\n");
    printTree(tree);

    // Teste
    printLeafNodes(tree);

    // Buscar chaves
    printf("\nBuscando chaves 10, 25, 40:\n");
    BPlusNode* result;

    result = searchKey(tree, 10);
    if (result) {
        printf("Chave 10 encontrada no nó com chaves: ");
        for (int i = 0; i < result->numKeys; i++) {
            printf("%d ", result->keys[i]);
        }
        printf("\n");
    } else {
        printf("Chave 10 não encontrada.\n");
    }

    result = searchKey(tree, 25);
    if (result) {
        printf("Chave 25 encontrada no nó com chaves: ");
        for (int i = 0; i < result->numKeys; i++) {
            printf("%d ", result->keys[i]);
        }
        printf("\n");
    } else {
        printf("Chave 25 não encontrada.\n");
    }

    result = searchKey(tree, 40);
    if (result) {
        printf("Chave 40 encontrada no nó com chaves: ");
        for (int i = 0; i < result->numKeys; i++) {
            printf("%d ", result->keys[i]);
        }
        printf("\n");
    } else {
        printf("Chave 40 não encontrada.\n");
    }

    // Remover chaves
    printf("\nRemovendo chave: 5\n");
    deleteKey(tree, 5);
    printf("\nÁrvore após exclusão do 5:\n");
    printTree(tree);
    printLeafNodes(tree);

    printf("\nRemovendo chave: 30\n");
    deleteKey(tree, 30);
    printf("\nÁrvore após exclusão do 30:\n");
    printTree(tree);

    return 0;
}
