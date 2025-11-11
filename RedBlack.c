#include <stdio.h>
#include <stdlib.h>
#include "RedBlack.h"

// Definição do ponteiro global para o sentinela.
No *NIL;

// === DEFINIÇÃO DOS CONTADORES GLOBAIS ===
long long g_count_insert = 0;
long long g_count_remove = 0;
long long g_count_search = 0;
long long g_count_search_min_max = 0;
long long g_count_rotations = 0;
// =========================================

// --- Inicialização ---

void InicializarNIL() {
    NIL = (No*)malloc(sizeof(No));
    if (!NIL) {
        fprintf(stderr, "Erro ao alocar NIL\n");
        exit(1);
    }
    NIL->cor = BLACK; 
    NIL->pai = NULL;
    NIL->esquerdo = NULL;
    NIL->direito = NULL;
    NIL->valor = 0;
    NIL->chave = 0;
}

No *CriarNo(long long valor, int chave) {
    No *n = (No*)malloc(sizeof(No));
    if (!n) {
        fprintf(stderr, "Erro alocar No\n");
        exit(1);
    }
    n->valor = valor;
    n->chave = chave;
    n->cor = RED;       // Novos nós são sempre VERMELHOS
    n->pai = NIL;
    n->esquerdo = NIL;
    n->direito = NIL;
    return n;
}

// --- Rotações ---

void LeftRotate(No **T, No *x) {
    No *y = x->direito; 
    x->direito = y->esquerdo; 
    if (y->esquerdo != NIL) {
        y->esquerdo->pai = x;
    }
    y->pai = x->pai; 
    if (x->pai == NIL) {
        *T = y; 
    } else if (x == x->pai->esquerdo) {
        x->pai->esquerdo = y;
    } else {
        x->pai->direito = y;
    }
    y->esquerdo = x; 
    x->pai = y;
    
    g_count_rotations++; // Incrementa o contador de rotação
}

void RightRotate(No **T, No *y) {
    No *x = y->esquerdo; 
    y->esquerdo = x->direito; 
    if (x->direito != NIL) {
        x->direito->pai = y;
    }
    x->pai = y->pai; 
    if (y->pai == NIL) {
        *T = x; 
    } else if (y == y->pai->direito) {
        y->pai->direito = x;
    } else {
        y->pai->esquerdo = x;
    }
    x->direito = y; 
    y->pai = x;
    
    g_count_rotations++; // Incrementa o contador de rotação
}

// --- Inserção ---

void RB_insert_fixup(No **T, No *z) {
    No *y; // Tio de z
    while (z->pai->cor == RED) {
        if (z->pai == z->pai->pai->esquerdo) { // Pai de z é filho esquerdo
            y = z->pai->pai->direito; // Tio
            // Caso 1: Tio 'y' é VERMELHO
            if (y->cor == RED) {
                z->pai->cor = BLACK;
                y->cor = BLACK;
                z->pai->pai->cor = RED;
                z = z->pai->pai; // Sobe na árvore
            } else {
                // Caso 2: Tio 'y' é PRETO, z é filho direito
                if (z == z->pai->direito) {
                    z = z->pai;
                    LeftRotate(T, z);
                }
                // Caso 3: Tio 'y' é PRETO, z é filho esquerdo
                z->pai->cor = BLACK;
                z->pai->pai->cor = RED;
                RightRotate(T, z->pai->pai);
            }
        } else { // Pai de z é filho direito (casos simétricos)
            y = z->pai->pai->esquerdo; // Tio
            // Caso 1: Tio 'y' é VERMELHO
            if (y->cor == RED) {
                z->pai->cor = BLACK;
                y->cor = BLACK;
                z->pai->pai->cor = RED;
                z = z->pai->pai;
            } else {
                // Caso 2: Tio 'y' é PRETO, z é filho esquerdo
                if (z == z->pai->esquerdo) {
                    z = z->pai;
                    RightRotate(T, z);
                }
                // Caso 3: Tio 'y' é PRETO, z é filho direito
                z->pai->cor = BLACK;
                z->pai->pai->cor = RED;
                LeftRotate(T, z->pai->pai);
            }
        }
    }
    (*T)->cor = BLACK; // Garante que a raiz seja PRETA
}

void RB_insert(No **T, No *z) {
    g_count_insert++; // Incrementa o contador de inserção
    
    No *y = NIL; 
    No *x = *T; 

    while (x != NIL) {
        y = x;
        if (z->valor < x->valor) {
            x = x->esquerdo;
        } else if (z->valor > x->valor) {
            x = x->direito;
        } else {
            // Se o valor já existe, apenas sobrescrevemos a 'chave' (tipo)
            x->chave = z->chave;
            free(z);
            return; // Não precisa de fixup
        }
    }

    z->pai = y;
    if (y == NIL) {
        *T = z; // Árvore estava vazia
    } else if (z->valor < y->valor) {
        y->esquerdo = z;
    } else {
        y->direito = z;
    }

    RB_insert_fixup(T, z);
}

// --- Remoção ---

void RB_transplant(No **T, No *u, No *v) {
    if (u->pai == NIL) {
        *T = v;
    } else if (u == u->pai->esquerdo) {
        u->pai->esquerdo = v;
    } else {
        u->pai->direito = v;
    }
    v->pai = u->pai;
}

No* Tree_minimum(No *n) {
    g_count_search_min_max++; // Incrementa o contador de busca min/max
    while (n->esquerdo != NIL) {
        n = n->esquerdo;
    }
    return n;
}

void RB_delete_fixup(No **T, No *x) {
    No *w; // Irmão de x
    while (x != *T && x->cor == BLACK) {
        if (x == x->pai->esquerdo) {
            w = x->pai->direito;
            // Caso 1: Irmão w é vermelho
            if (w->cor == RED) {
                w->cor = BLACK;
                x->pai->cor = RED;
                LeftRotate(T, x->pai);
                w = x->pai->direito;
            }
            // Caso 2: Irmão w é preto, ambos os filhos de w são pretos
            if (w->esquerdo->cor == BLACK && w->direito->cor == BLACK) {
                w->cor = RED;
                x = x->pai;
            } else {
                // Caso 3: Irmão w é preto, filho esq é vermelho, filho dir é preto
                if (w->direito->cor == BLACK) {
                    w->esquerdo->cor = BLACK;
                    w->cor = RED;
                    RightRotate(T, w);
                    w = x->pai->direito;
                }
                // Caso 4: Irmão w é preto, filho dir é vermelho
                w->cor = x->pai->cor;
                x->pai->cor = BLACK;
                w->direito->cor = BLACK;
                LeftRotate(T, x->pai);
                x = *T; // Termina o loop
            }
        } else {
            // Casos simétricos (x é filho direito)
            w = x->pai->esquerdo;
            // Caso 1
            if (w->cor == RED) {
                w->cor = BLACK;
                x->pai->cor = RED;
                RightRotate(T, x->pai);
                w = x->pai->esquerdo;
            }
            // Caso 2
            if (w->direito->cor == BLACK && w->esquerdo->cor == BLACK) {
                w->cor = RED;
                x = x->pai;
            } else {
                // Caso 3
                if (w->esquerdo->cor == BLACK) {
                    w->direito->cor = BLACK;
                    w->cor = RED;
                    LeftRotate(T, w);
                    w = x->pai->esquerdo;
                }
                // Caso 4
                w->cor = x->pai->cor;
                x->pai->cor = BLACK;
                w->esquerdo->cor = BLACK;
                RightRotate(T, x->pai);
                x = *T; // Termina o loop
            }
        }
    }
    x->cor = BLACK;
}

void RB_delete(No **T, No *z) {
    g_count_remove++; // Incrementa o contador de remoção

    No *y = z; 
    No *x;     
    int y_original_color = y->cor;

    if (z->esquerdo == NIL) {
        x = z->direito;
        RB_transplant(T, z, z->direito);
    } else if (z->direito == NIL) {
        x = z->esquerdo;
        RB_transplant(T, z, z->esquerdo);
    } else {
        y = Tree_minimum(z->direito); // y é o sucessor de z
        y_original_color = y->cor;
        x = y->direito;
        if (y->pai == z) {
            x->pai = y; // Necessário se x for NIL
        } else {
            RB_transplant(T, y, y->direito);
            y->direito = z->direito;
            y->direito->pai = y;
        }
        RB_transplant(T, z, y);
        y->esquerdo = z->esquerdo;
        y->esquerdo->pai = y;
        y->cor = z->cor;
    }

    if (y_original_color == BLACK) {
        RB_delete_fixup(T, x);
    }

    free(z); // Libera a memória do nó original
}

// --- Funções Auxiliares (Busca e Desalocação) ---

No* BuscarNo(No *n, long long valor) {
    g_count_search++; // Incrementa o contador de busca
    
    while (n != NIL && valor != n->valor) {
        if (valor < n->valor)
            n = n->esquerdo;
        else
            n = n->direito;
    }
    return n; // Retorna NIL se não encontrar
}

No* BuscarMaximo(No *n) {
    g_count_search_min_max++; // Incrementa o contador de busca min/max

    if (n == NIL) return NULL; 
    
    while (n->direito != NIL) {
        n = n->direito;
    }
    
    return (n == NIL) ? NULL : n;
}

void DesalocarArvore(No *n) {
    if (n == NIL) {
        return;
    }
    DesalocarArvore(n->esquerdo);
    DesalocarArvore(n->direito);
    free(n);
}