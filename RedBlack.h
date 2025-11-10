#ifndef REDBLACK_H
#define REDBLACK_H

#define RED 1
#define BLACK 0

typedef struct No {
    long long valor; // valor usado como chave de ordenação (ex: id do item)
    int chave;        // categoria ou tipo
    int cor;          // RED ou BLACK
    struct No *pai;
    struct No *esquerdo;
    struct No *direito;
} No;
 
No *InserirNoRec(long long valor, int chave, No *raiz);
No *RemoverNoRec(long long valor, No *raiz);

void DesalocarArvore(No *raiz);
#endif