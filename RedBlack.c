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

    // ajustar pais
    x->pai = h->pai;
    h->pai = x;
    // cores
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

// balanceio local
No *balancear(No *h) {
    if (h == NULL) return NULL;
    if (ehVermelho(h->direito) && !ehVermelho(h->esquerdo)) h = RotateLeft(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->esquerdo->esquerdo)) h = RotateRight(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->direito)) FlipColors(h);
    return h;
}

// move vermelho para esquerda (usado em remoção)
No *MoverVermelhoEsquerda(No *h) {
    FlipColors(h);
    if (h->direito && ehVermelho(h->direito->esquerdo)) {
        h->direito = RotateRight(h->direito);
        if (h->direito) h->direito->pai = h;
        h = RotateLeft(h);
        FlipColors(h);
    }
    return h;
}

// move vermelho para direita (usado em remoção)
No *MoverVermelhoDireita(No *h) {
    FlipColors(h);
    if (h->esquerdo && ehVermelho(h->esquerdo->esquerdo)) {
        h = RotateRight(h);
        FlipColors(h);
    }
    return h;
}

No *AcharMinimo(No *raiz) {
    if (!raiz) return NULL;
    No *cur = raiz;
    while (cur->esquerdo) cur = cur->esquerdo;
    return cur;
}

// remover nó mínimo preservando subárvore direita
No *removerNoMinimo(No *h) {
    if (!h) return NULL;
    if (h->esquerdo == NULL) {
        No *direita = h->direito;
        free(h);
        if (direita) direita->pai = NULL;
        return direita;
    }
    if (!ehVermelho(h->esquerdo) && !ehVermelho(h->esquerdo->esquerdo)) {
        h = MoverVermelhoEsquerda(h);
    }
    h->esquerdo = removerNoMinimo(h->esquerdo);
    if (h->esquerdo) h->esquerdo->pai = h;
    return balancear(h);
}

No *RemoverMinimo(No *raiz) {
    if (!raiz) return NULL;
    raiz = removerNoMinimo(raiz);
    if (raiz) { raiz->cor = BLACK; raiz->pai = NULL; }
    return raiz;
}

// inserção recursiva (retorna raiz da subárvore)
No *InserirNoRec(long long valor, int chave, No *h) {
    if (h == NULL) return CriarNo(valor, chave);

    if (valor < h->valor) {
        No *e = InserirNoRec(valor, chave, h->esquerdo);
        h->esquerdo = e;
        if (e) e->pai = h;
    } else {
        No *d = InserirNoRec(valor, chave, h->direito);
        h->direito = d;
        if (d) d->pai = h;
    }

    if (ehVermelho(h->direito) && !ehVermelho(h->esquerdo)) h = RotateLeft(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->esquerdo->esquerdo)) h = RotateRight(h);
    if (ehVermelho(h->esquerdo) && ehVermelho(h->direito)) FlipColors(h);

    return h;
}

No *InserirNo(long long valor, int chave, No *raiz) {
    raiz = InserirNoRec(valor, chave, raiz);
    if (raiz) { raiz->cor = BLACK; raiz->pai = NULL; }
    return raiz;
}

// tamanho (recursivo)
int tamanho_recursivo(No *raiz) {
    if (!raiz) return 0;
    return 1 + tamanho_recursivo(raiz->esquerdo) + tamanho_recursivo(raiz->direito);
}
int Tamanho(No *raiz) { return tamanho_recursivo(raiz); }

// procura o nó com chave == chave_evento e menor valor (próximo tick)
No* AcharEvento(No* raiz, int chave_evento) {
    if (!raiz) return NULL;
    No *melhor = NULL;
    if (raiz->chave == chave_evento) melhor = raiz;
    No *esq = AcharEvento(raiz->esquerdo, chave_evento);
    if (esq && (!melhor || esq->valor < melhor->valor)) melhor = esq;
    No *dir = AcharEvento(raiz->direito, chave_evento);
    if (dir && (!melhor || dir->valor < melhor->valor)) melhor = dir;
    return melhor;
}