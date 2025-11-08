#include <stdio.h>
#include <stdlib.h>

#define RED 1
#define BLACK 0

typedef struct no{
    int cor;
    struct no *pai;
    struct no *esquerdo;
    struct no *direito;
    int valor;
    int chave;

}No;

No *CriarNo(int valor, int chave){
    No *raiz = malloc(sizeof(No))

    if(raiz == NULL){
        printf("Erro ao alocar memória");
        exit(1);
    }

    raiz->cor = RED;
    raiz->esquerdo = NULL;
    raiz->direito = NULL;
    raiz->pai = NULL;
    raiz->valor = valor;
    raiz->chave = chave;
    return raiz;
}


No *InserirNo(int valor, int chave, No *raiz){
    if(raiz == NULL){
        raiz = CriarNo(valor, chave);
        return raiz;
    }
    if(valor > raiz->valor){
        raiz->direito = InserirNo(valor, chave, raiz->direito);
        raiz->direito->pai = raiz;
    }

    if(valor < raiz->valor){
        raiz->esquerdo = InserirNo(valor, chave, raiz->esquerdo);
        raiz->esquerdo->pai = raiz;
    }

    if(raiz->direito->cor ==RED){
        raiz = RotateLeft(raiz);
    }
    if(raiz->cor == RED && raiz->esquerdo->cor == RED){
        raiz = RotateRight(raiz);
    }
    if(raiz->esquerdo->cor == RED && raiz->direito->cor == RED){
        raiz = FlipColors(raiz);
    }
    return raiz;
}

No *RotateLeft(No *raiz){
    No *aux = raiz->direito;
    raiz->direito = aux->esquerdo;
    aux->esquerdo = raiz;
    aux->cor = raiz->cor;
    raiz->cor = RED;
    return aux;
}

No *RotateRight(No *raiz){
    No *aux = raiz->esquerdo;
    raiz->esquerdo = aux->direito;
    aux->direito = raiz;
    aux->cor = raiz->cor;
    raiz->cor = RED;
    return aux;
}

void FlipColors(No *raiz){
    raiz->cor = !raiz->cor;
    raiz->esquerdo->cor = !raiz->esquerdo->cor;
    raiz->direito->cor = !raiz->direito->cor;
}
