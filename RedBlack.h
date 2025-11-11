#ifndef REDBLACK_H
#define REDBLACK_H

#define RED 1
#define BLACK 0

typedef struct No {
    long long valor; // Chave de ordenação
    int chave;       // Categoria (tipo de emoção)
    int cor;
    struct No *pai;
    struct No *esquerdo;
    struct No *direito;
} No;

// Nó sentinela global
extern No *NIL;

// === NOVOS CONTADORES GLOBAIS ===
// (Serão definidos em RedBlack.c e lidos em main.c)
extern long long g_count_insert;
extern long long g_count_remove;
extern long long g_count_search;
extern long long g_count_search_min_max;
extern long long g_count_rotations;
// ================================

// Funções principais
void InicializarNIL();
No *CriarNo(long long valor, int chave);
void RB_insert(No **T, No *z);
void RB_delete(No **T, No *z);

// Funções de busca (para o jogo)
No* BuscarNo(No *n, long long valor); 
No* BuscarMaximo(No *n);
No* Tree_minimum(No *n);

void DesalocarArvore(No *n);

#endif