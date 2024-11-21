Implementar uma árvore B+ (2-3) (no máximo duas chaves e 3 ponteiros por nó).
TAD: Inclusão de chave na árvore (cisão)
Exclusão de chave da árvore (concatenação e redistribuição)
Busca de uma chave, retornando o ponteiro para o nó onde a chave se encontra ou
NULL(se a chave não for encontrada na estrutura)
Impressão da árvore completa (ptr para o nó, conteúdo do nó – chave1, chave2, ptr1,
ptr2, ptr3)
Estrutura do nó:
ptr1 ch1 ptr2 ch2 ptr3
Obs: As folhas não precisam estar encadeadas.
As chaves que constam dos nós internos devem estar presentes no nó-folha da direita.
As chaves são números inteiros (quando o nó não contiver alguma chave, exibir como
chave o valor -1).

🚨🚨🚨🚨🚨🚨
Oi, Pedro
Acho que a inserção e busca estão funcionando (precisa de mais testes).
A remoção com certeza não está, acredito que o problema pode estar em uma (ou mais) das seguintes funções:
deleteKey
void handleUnderflow(BPlusTree* tree, BPlusNode* node);
void redistributeFromLeft(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* leftSibling, int nodeIndex);
void redistributeFromRight(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* rightSibling, int nodeIndex);
void mergeWithLeft(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* leftSibling, int nodeIndex);
void mergeWithRight(BPlusTree* tree, BPlusNode* parent, BPlusNode* node, BPlusNode* rightSibling, int nodeIndex);
🚨🚨🚨🚨🚨🚨
