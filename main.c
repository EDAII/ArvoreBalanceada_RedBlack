#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RedBlack.h"

typedef enum { FELICIDADE, TRISTEZA, RAIVA, MEDO } TipoEmocao;

typedef struct {
    long long id;
    TipoEmocao tipo;
    int intensidade;
} Emocao;

No *raiz = NULL;
long long next_id = 1;
int idade = 0;

// Função para gerar intensidade aleatória dentro do intervalo
int gerar_intensidade(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Função para adicionar emoção à RB Tree
void adicionar_emocao(TipoEmocao tipo, int intensidade) {
    Emocao *e = malloc(sizeof(Emocao));
    e->id = next_id++;
    e->tipo = tipo;
    e->intensidade = intensidade;

    raiz = InserirNoRec(intensidade, tipo, raiz);
    printf("-> Emoção gerada: tipo %d, intensidade %d\n", tipo, intensidade);
}

// Busca emoção dominante (maior intensidade)
No* emocao_dominante() {
    No *atual = raiz;
    if (!atual) return NULL;
    while(atual->direito) atual = atual->direito;
    return atual;
}

// Evento de vida da criança (1-10 anos)
void evento(int idade) {
    printf("\n[Idade %d] Um evento ocorre com a criança!\n", idade);

    // Mock de 20 eventos possíveis
    const char *opcoes[20] = {
        "Dar um doce", "Ignorar", "Bater", "Ajudar na lição", "Deixar sozinho", 
        "Gritar", "Brincar com ela", "Ficar ocupado", "Punir injustamente",
        "Elogiá-la", "Dizer algo motivador", "Discutir na frente da criança",
        "Levar ao parque", "Ficar no celular", "Comprar um brinquedo",
        "Negar brinquedo", "Dar abraço", "Repreender calmamente", 
        "Contar história", "Ignorar birra"
    };

    int escolha = rand() % 20;
    printf("Evento: %s\n", opcoes[escolha]);

    // Cada escolha gera de 1 a 2 emoções com intensidade randômica
    int num_emocoes = 1 + rand() % 2;
    for(int i=0;i<num_emocoes;i++){
        TipoEmocao tipo = rand() % 4; // aleatório
        int intensidade = gerar_intensidade(10,30); // intensidade entre 10 e 30
        adicionar_emocao(tipo, intensidade);
    }

    // Chance de evento extremo
    if(rand()%20==0){
        int intensidade = gerar_intensidade(150,200);
        adicionar_emocao(TRISTEZA, intensidade);
        printf("-> Evento extremo! Emoção intensa gerada: TRISTEZA %d\n", intensidade);
    }
}

int main() {
    srand(time(NULL));
    printf("=== Jogo: Criando a Criança (1-10 anos) ===\n");

    for(idade=1; idade<=10; idade++) {
        evento(idade);

        // Emoção dominante
        No *dominante = emocao_dominante();
        if(dominante){
            printf("Emoção dominante atual: tipo %d, intensidade %lld\n", dominante->chave, dominante->valor);
            if(dominante->valor > 100){
                printf("Emoção extrema detectada! A criança entrou em crise. Fim de jogo.\n");
                break;
            }
        }
    }

    printf("\nFim da infância da criança.\n");
    DesalocarArvore(raiz);
    return 0;
}
