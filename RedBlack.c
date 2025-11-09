#include <stdio.h>
#include <stdlib.h>
#include "RedBlack.h"

No *CriarNo(int valor, int chave){
    No *raiz = malloc(sizeof(No));

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

    if(raiz->direito != NULL && raiz->direito->cor ==RED){
        raiz = RotateLeft(raiz);
    }
    if(raiz->esquerdo != NULL && raiz->esquerdo->cor == RED && raiz->esquerdo->esquerdo != NULL && raiz->esquerdo->esquerdo->cor == RED){
        raiz = RotateRight(raiz);
    }
    if(raiz->esquerdo != NULL && raiz->direito != NULL && raiz->esquerdo->cor == RED && raiz->direito->cor == RED){
        FlipColors(raiz);
    }
    return raiz;
}

int ehVermelho(No *h){
    if(h == NULL) return 0;
    return h->cor ==RED;
}

No *balancear(No *raiz){
    if (ehVermelho(raiz->direito) && !ehVermelho(raiz->esquerdo)) raiz = RotateLeft(raiz);
    if(ehVermelho(raiz->esquerdo) && ehVermelho(raiz->esquerdo->esquerdo)) raiz = RotateRight(raiz);
    if(ehVermelho(raiz->esquerdo) && ehVermelho(raiz->direito)) FlipColors(raiz);

    return raiz;
}

No *MoverVermelhoEsquerda(No *raiz){
    FlipColors(raiz);

    if (raiz->direito != NULL && ehVermelho(raiz->direito->esquerdo)){
        raiz->direito = RotateRight(raiz->direito);
        raiz = RotateLeft(raiz);
        FlipColors(raiz);
    }
    return raiz;
}

No *MoverVermelhoDireita(No *raiz){
    FlipColors(raiz);

    if (raiz->esquerdo != NULL && ehVermelho(raiz->esquerdo->esquerdo)){
        raiz = RotateRight(raiz);
        FlipColors(raiz);
    }
    return raiz;
}

No *AcharMinimo(No *raiz){
    if(raiz->esquerdo== NULL) return raiz;
    return AcharMinimo(raiz->esquerdo);
}

No *RemoverMinimo(No *raiz){
    if(raiz->esquerdo == NULL) {
        free(raiz);
        return NULL;
    }

    if(!ehVermelho(raiz->esquerdo) && !ehVermelho(raiz->esquerdo->esquerdo)){
        raiz = MoverVermelhoEsquerda(raiz);
    }

    raiz->esquerdo = RemoverMinimo(raiz->esquerdo);

    return balancear(raiz);
}

No *removerNo(No *raiz, int valor){
    if(raiz == NULL) return NULL;

    if(valor < raiz->valor){
        if(raiz->esquerdo != NULL && !ehVermelho(raiz->esquerdo) && !ehVermelho(raiz->esquerdo->esquerdo)){
            raiz = MoverVermelhoEsquerda(raiz);
        }
        raiz->esquerdo = removerNo(raiz->esquerdo, valor);

    } else {
        if(ehVermelho(raiz->esquerdo)){
            raiz = RotateRight(raiz);
        }

        if(valor == raiz->valor && raiz->direito == NULL){
            free(raiz);
            return NULL;
        }

        if(raiz->direito != NULL && !ehVermelho(raiz->direito) && !ehVermelho(raiz->direito->esquerdo)){
            raiz = MoverVermelhoDireita(raiz);
        }

        if(valor == raiz->valor){
            No *sucessor = AcharMinimo(raiz->direito);
            raiz->valor = sucessor->valor;
            raiz->chave = sucessor->chave;
            raiz->direito = RemoverMinimo(raiz->direito);
        } else {
            raiz->direito = removerNo(raiz->direito, valor);
        }
    }

    return balancear(raiz);
}

No *Remover(No *raiz, int valor){
    if(raiz == NULL) return NULL;

    raiz = removerNo(raiz, valor);

    if(raiz != NULL) raiz->cor = BLACK;
    
    return raiz;
}
