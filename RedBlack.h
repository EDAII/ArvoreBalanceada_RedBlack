#ifndef ARVORE_H
#define ARVORE_H

typedef struct no{
    int cor;
    struct no *pai;
    struct no *esquerdo;
    struct no *direito;
    int valor;
    int chave;

}No;

No *CriarNo(int valor, int chave);
No *InserirNo(int valor, int chave, No *raiz);
No *RotateLeft(No *raiz);
No *RotateRight(No *raiz);
void FlipColors(No *raiz);
No *Remover(No *raiz, int valor);
No *removerNo(No *raiz, int valor);
No *RemoverMinimo(No *raiz);
No *balancear(No *raiz);
No *MoverVermelhoEsquerda(No *raiz);
No *MoverVermelhoDireita(No *raiz);
No *AcharMinimo(No *raiz);
int ehVermelho(No *h);


#endif