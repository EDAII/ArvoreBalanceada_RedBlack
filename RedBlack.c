#include <stdio.h>
#include <stdlib.h>
#include "RedBlack.h"

// cria nó
No *CriarNo(long long valor, int chave) {
    No *n = malloc(sizeof(No));
    if (!n) {
        fprintf(stderr, "Erro alocar No\n");
        exit(1);
    }
    n->cor = RED;
    n->pai = NULL;
    n->esquerdo = NULL;
    n->direito = NULL;
    n->valor = valor;
    n->chave = chave;
    return n;
}

int ehVermelho(No *h) {
    if (h == NULL) return 0;
    return h->cor == RED;
}

No *RotateLeft(No *h) {
    if (!h || !h->direito) return h;
    No *x = h->direito;
    h->direito = x->esquerdo;
    if (x->esquerdo) x->esquerdo->pai = h;
    x->esquerdo = h;
    x->pai = h->pai;
    h->pai = x;
    x->cor = h->cor;
    h->cor = RED;
    return x;
}

No *RotateRight(No *h) {
    if (!h || !h->esquerdo) return h;
    No *x = h->esquerdo;
    h->esquerdo = x->direito;
    if (x->direito) x->direito->pai = h;
    x->direito = h;
    x->pai = h->pai;
    h->pai = x;
    x->cor = h->cor;
    h->cor = RED;
    return x;
}

void FlipColors(No *h) {
    if (!h || !h->esquerdo || !h->direito) return;
    h->cor = !h->cor;
    h->esquerdo->cor = !h->esquerdo->cor;
    h->direito->cor = !h->direito->cor;
}

No *balancear(No *h) {
    if (h == NULL) return NULL;
    if (ehVermelho(h->direito) && !ehVermelho(h->esquerdo))
        h = RotateLeft(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->esquerdo->esquerdo))
        h = RotateRight(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->direito))
        FlipColors(h);
    return h;
}

// inserção recursiva
No *InserirNoRec(long long valor, int chave, No *h) {
    if (h == NULL) return CriarNo(valor, chave);

    if (valor < h->valor) {
        No *e = InserirNoRec(valor, chave, h->esquerdo);
        h->esquerdo = e;
        if (e) e->pai = h;
    } else if (valor > h->valor) {
        No *d = InserirNoRec(valor, chave, h->direito);
        h->direito = d;
        if (d) d->pai = h;
    } else {
        h->chave = chave; // sobrescreve chave se valor igual
    }

    if (ehVermelho(h->direito) && !ehVermelho(h->esquerdo)) h = RotateLeft(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->esquerdo->esquerdo)) h = RotateRight(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->direito)) FlipColors(h);
    return h;
}

No *remover_min(No *h, No **min) {
    if (!h->esquerdo) {
        *min = h;
        return h->direito;
    }
    h->esquerdo = remover_min(h->esquerdo, min);
    return balancear(h);
}

No *RemoverNoRec(long long valor, No *h) {
    if (!h) return NULL;
    if (valor < h->valor) h->esquerdo = RemoverNoRec(valor,h->esquerdo);
    else if (valor > h->valor) h->direito = RemoverNoRec(valor,h->direito);
    else {
        if (!h->direito) return h->esquerdo;
        if (!h->esquerdo) return h->direito;
        No *min = NULL;
        No *d = h->direito;
        d = remover_min(d, &min);
        min->esquerdo = h->esquerdo;
        min->direito = d;
        min->cor = h->cor;
        free(h);
        return balancear(min);
    }
    return balancear(h);
}


void DesalocarArvore(No *raiz){
    if (raiz == NULL) return;
    DesalocarArvore(raiz->esquerdo);
    DesalocarArvore(raiz->direito);
    free(raiz);
}