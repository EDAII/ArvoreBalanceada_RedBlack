#ifndef ARVORE_H
#define ARVORE_H

// cores
#define RED 1
#define BLACK 0

typedef struct no {
    int cor;
    struct no *pai;
    struct no *esquerdo;
    struct no *direito;
    long long valor; // timestamp em ms
    int chave;       // id do evento
} No;

// protótipos
No *CriarNo(long long valor, int chave);
No *InserirNo(long long valor, int chave, No *raiz);
No *RotateLeft(No *raiz);
No *RotateRight(No *raiz);
void FlipColors(No *raiz);

No *RemoverMinimo(No *raiz);
No *AcharMinimo(No *raiz);
int ehVermelho(No *h);

int Tamanho(No *raiz);
No* AcharEvento(No* raiz, int chave_evento);

#endif