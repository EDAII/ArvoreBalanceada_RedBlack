#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include "RedBlack.h"

// --- EVENTOS ---
#define EVENTO_FOGO_CD 1
#define EVENTO_CURA_CD 2
#define EVENTO_ACELERAR_CD 3
#define EVENTO_VENENO_CD 4
#define EVENTO_FADIGA_CD 5
#define EVENTO_INIMIGO_STUN_FIM 6



#define EVENTO_INIMIGO_VENENO_TICK 10
#define EVENTO_JOGADOR_ACELERAR_FIM 11
#define EVENTO_INIMIGO_ATAQUE_CARREGADO 12

#define EVENTO_TURNO_JOGADOR_PRONTO 30
#define EVENTO_TURNO_INIMIGO_PRONTO 31

// estado global
long long tick_atual = 0;
No *arvore_eventos = NULL;
int eventos_na_arvore = 0;
char log_batalha[256] = "Um Behemoth surge!";


// ATB display
long long atb_jogador_inicio = 0, atb_jogador_fim = 0;
long long atb_inimigo_inicio = 0, atb_inimigo_fim = 0;

// jogador
int vida_jogador = 100;
int mana_jogador = 50;
int velocidade_jogador = 10;
int recarga_fogo = 0, recarga_cura = 0, recarga_acelerar = 0, recarga_veneno = 0;

// controle de efeitos do jogador
int recarga_fadiga = 0;    // jogador está cansado (golpe fadigante em cooldown)
int inimigo_stun = 0;      // inimigo está atordoado

// inimigo
int vida_inimigo = 200;
int velocidade_inimigo = 8;
int inimigo_ticks_veneno = 0;

// debug file
FILE *debugLog = NULL;

long long tempo_real_ms() {
    struct timespec spec; clock_gettime(CLOCK_REALTIME, &spec);
    return (long long)spec.tv_sec * 1000 + (long long)(spec.tv_nsec / 1000000);
}

long long calcular_tick_atb(int velocidade) {
    if (velocidade <= 0) velocidade = 1;
    return tick_atual + (10000 / velocidade);
}

// desenho
void desenhar_barra_colorida(int y, int x, int valor_atual, int valor_max, int largura, int color) {
    if (valor_max <= 0) valor_max = 1;
    int preenchido = (int)(((float)valor_atual / (float)valor_max) * largura);
    if (preenchido < 0) preenchido = 0;
    if (preenchido > largura) preenchido = largura;
    attron(COLOR_PAIR(color));
    mvprintw(y, x, "[");
    for (int i = 0; i < largura; ++i) mvprintw(y, x + 1 + i, i < preenchido ? "|" : " ");
    mvprintw(y, x + 1 + largura, "]");
    attroff(COLOR_PAIR(color));
}

void desenhar_barra_atb(int y, int x, long long ini, long long fim, int color) {
    int largura = 20;
    int preenchido = 0;
    long long dur = fim - ini;
    if (dur <= 0) dur = 1;
    long long passado = tick_atual - ini;
    preenchido = (int)(((float)passado / (float)dur) * largura);
    if (preenchido < 0) preenchido = 0;
    if (preenchido > largura) preenchido = largura;
    attron(COLOR_PAIR(color));
    mvprintw(y, x, "[");
    for (int i = 0; i < largura; ++i) mvprintw(y, x + 1 + i, i < preenchido ? "|" : " ");
    mvprintw(y, x + 1 + largura, "]");
    attroff(COLOR_PAIR(color));
}

typedef enum { JOGO_RODANDO, MENU_ACAO_ABERTO, MENU_MAGIA_ABERTO } EstadoJogo;
EstadoJogo estado_atual = JOGO_RODANDO;
int cursor_menu = 0;

void desenhar_interface() {
    mvprintw(1, 3, "=== BATALHA (Chrono Trigger style) ===");
    mvprintw(3, 3, "JOGADOR");
    mvprintw(4, 3, "HP: %3d/100  MP: %2d/50", vida_jogador, mana_jogador);
    desenhar_barra_colorida(4, 26, vida_jogador, 100, 20, 2);
    desenhar_barra_atb(6, 3, atb_jogador_inicio, atb_jogador_fim, 2);

    mvprintw(3, 50, "INIMIGO - Behemoth");
    mvprintw(4, 50, "HP: %3d/200", vida_inimigo);
    desenhar_barra_colorida(4, 75, vida_inimigo, 200, 20, 1);
    desenhar_barra_atb(6, 50, atb_inimigo_inicio, atb_inimigo_fim, 1);

    mvprintw(9, 3, "LOG: %s", log_batalha);
    mvprintw(0, 60, "EVENTOS: %d", eventos_na_arvore);
    mvprintw(11, 3, "Controles: a=ataque m=magia f/h/v/a=atalhos magia q=sair");
    mvprintw(12, 3, "Chrono Trigger: o tempo pausa enquanto menus estiverem abertos.");
}

// ações
No* schedule_turn_player(No* arvore) {
    long long proximo = calcular_tick_atb(velocidade_jogador);
    arvore = InserirNo(proximo, EVENTO_TURNO_JOGADOR_PRONTO, arvore);
    atb_jogador_inicio = tick_atual;
    atb_jogador_fim = proximo;
    if (debugLog) fprintf(debugLog, "[%lld] Inserido TURNO_JOGADOR -> %lld\n", tick_atual, proximo);
    return arvore;
}

No* schedule_turn_enemy(No* arvore) {
    long long proximo = calcular_tick_atb(velocidade_inimigo);
    arvore = InserirNo(proximo, EVENTO_TURNO_INIMIGO_PRONTO, arvore);
    atb_inimigo_inicio = tick_atual;
    atb_inimigo_fim = proximo;
    if (debugLog) fprintf(debugLog, "[%lld] Inserido TURNO_INIMIGO -> %lld\n", tick_atual, proximo);
    return arvore;
}

No* executar_acao_jogador(No* arvore, int acao) {
    if (acao == 0) { // pancada básica
        int dano = 10 + (rand() % 7);
        vida_inimigo -= dano;

        // chance de stun: 30%
        int chance = rand() % 100;
        if (chance < 30) {
            inimigo_stun = 1;
            arvore = InserirNo(tick_atual + 2500, EVENTO_INIMIGO_STUN_FIM, arvore);
            sprintf(log_batalha, "Pancada! %d dmg e o inimigo ficou atordoado!", dano);
            if (debugLog) fprintf(debugLog, "[%lld] Stun causado (pancada)\n", tick_atual);
        } else {
            sprintf(log_batalha, "Pancada! %d de dano.", dano);
        }

        if (debugLog) fprintf(debugLog, "[%lld] Jogador pancada %d\n", tick_atual, dano);
    }

    else if (acao == 1) { // golpe fadigante
        if (recarga_fadiga) {
            sprintf(log_batalha, "Você ainda está cansado!");
            return arvore;
        }

        int dano = 25 + (rand() % 11);
        vida_inimigo -= dano;
        inimigo_stun = 1;  // sempre causa stun
        recarga_fadiga = 1;

        sprintf(log_batalha, "GOLPE FADIGANTE! %d dmg e o inimigo está atordoado!", dano);
        arvore = InserirNo(tick_atual + 4000, EVENTO_FADIGA_CD, arvore);         // cooldown de fadiga
        arvore = InserirNo(tick_atual + 3000, EVENTO_INIMIGO_STUN_FIM, arvore);  // stun dura 3s

        if (debugLog) fprintf(debugLog, "[%lld] Fadigante: dano=%d, stun aplicado\n", tick_atual, dano);
    }

    // re-agenda o turno
    arvore = schedule_turn_player(arvore);
    estado_atual = JOGO_RODANDO;
    return arvore;
}


No* executar_magia_jogador(No* arvore, int magia) {
    int gasto = 0;
    switch(magia) {
        case 0: // fogo
            gasto = 10;
            if (recarga_fogo || mana_jogador < gasto) {
                sprintf(log_batalha, recarga_fogo ? "Fogo em cooldown!" : "Sem mana!");
                return arvore;
            }
            mana_jogador -= gasto; recarga_fogo = 1;
            vida_inimigo -= 30;
            sprintf(log_batalha, "BOLA DE FOGO! -30 HP");
            // CD do fogo (5s)
            arvore = InserirNo(tick_atual + 5000, EVENTO_FOGO_CD, arvore);
            if (debugLog) fprintf(debugLog, "[%lld] Inserido FOGO_CD @%lld\n", tick_atual, tick_atual+5000);
            break;
        case 1: // cura
            gasto = 15;
            if (recarga_cura || mana_jogador < gasto) {
                sprintf(log_batalha, recarga_cura ? "Cura em cooldown!" : "Sem mana!");
                return arvore;
            }
            mana_jogador -= gasto; recarga_cura = 1;
            vida_jogador += 50; if (vida_jogador > 100) vida_jogador = 100;
            sprintf(log_batalha, "CURA! +50 HP");
            arvore = InserirNo(tick_atual + 8000, EVENTO_CURA_CD, arvore);
            if (debugLog) fprintf(debugLog, "[%lld] Inserido CURA_CD @%lld\n", tick_atual, tick_atual+8000);
            break;
        case 2: // veneno
            gasto = 10;
            if (recarga_veneno || mana_jogador < gasto) {
                sprintf(log_batalha, recarga_veneno ? "Veneno em cooldown!" : "Sem mana!");
                return arvore;
            }
            mana_jogador -= gasto; recarga_veneno = 1;
            // aplica primeiro tick imediato (Chrono Trigger: efeitos podem bater na hora)
            inimigo_ticks_veneno = 3;
            if (inimigo_ticks_veneno > 0) {
                vida_inimigo -= 10;
                inimigo_ticks_veneno--;
                sprintf(log_batalha, "Veneno: -10 HP (imediato)");
                if (debugLog) fprintf(debugLog, "[%lld] Veneno tick imediato\n", tick_atual);
            }
            // agenda próximos ticks espaçados 1500ms
            if (inimigo_ticks_veneno > 0) arvore = InserirNo(tick_atual + 1500, EVENTO_INIMIGO_VENENO_TICK, arvore);
            if (inimigo_ticks_veneno > 1) arvore = InserirNo(tick_atual + 3000, EVENTO_INIMIGO_VENENO_TICK, arvore);
            // agenda cooldown do veneno
            arvore = InserirNo(tick_atual + 10000, EVENTO_VENENO_CD, arvore);
            if (debugLog) fprintf(debugLog, "[%lld] Inserido VENENO_CD e VENENO_TICKS\n", tick_atual);
            break;
        case 3: // acelerar
            gasto = 8;
            if (recarga_acelerar || mana_jogador < gasto) {
                sprintf(log_batalha, recarga_acelerar ? "Acelerar em cooldown!" : "Sem mana!");
                return arvore;
            }
            mana_jogador -= gasto; recarga_acelerar = 1;
            velocidade_jogador = 20;
            sprintf(log_batalha, "ACELERAR ativo por 10s");
            arvore = InserirNo(tick_atual + 10000, EVENTO_ACELERAR_CD, arvore);
            arvore = InserirNo(tick_atual + 10000, EVENTO_JOGADOR_ACELERAR_FIM, arvore);
            if (debugLog) fprintf(debugLog, "[%lld] Inserido ACELERAR_CD e FIM @%lld\n", tick_atual, tick_atual+10000);
            break;
        
    }

    // após magia, re-agenda turno do jogador
    arvore = schedule_turn_player(arvore);
    estado_atual = JOGO_RODANDO;
    return arvore;
}

No* executar_ia_inimigo(No* arvore) {
    if (inimigo_stun) {
        sprintf(log_batalha, "Behemoth está atordoado!");
        if (debugLog) fprintf(debugLog, "[%lld] Inimigo perdeu o turno (stun)\n", tick_atual);
        inimigo_stun = 0;  // remove o stun se o turno chegou antes do evento
        arvore = schedule_turn_enemy(arvore);
        return arvore;
    }

    int dano = 10 + (rand() % 7);
    vida_jogador -= dano;
    sprintf(log_batalha, "Behemoth ataca! %d de dano.", dano);
    if (debugLog) fprintf(debugLog, "[%lld] Inimigo ataca %d\n", tick_atual, dano);

    arvore = schedule_turn_enemy(arvore);
    return arvore;
}


// processa eventos prontos
No *processar_eventos(No *arvore) {
    while (1) {
        No *min = AcharMinimo(arvore);
        if (!min || min->valor > tick_atual) break;

        int chave = min->chave;
        // remove antes de processar
        arvore = RemoverMinimo(arvore);

        if (debugLog) fprintf(debugLog, "[%lld] Disparando evento %d\n", tick_atual, chave);

        switch (chave) {
            case EVENTO_FOGO_CD: recarga_fogo = 0; if (debugLog) fprintf(debugLog, "[%lld] FOGO_CD reset\n", tick_atual); break;
            case EVENTO_CURA_CD: recarga_cura = 0; if (debugLog) fprintf(debugLog, "[%lld] CURA_CD reset\n", tick_atual); break;
            case EVENTO_VENENO_CD: recarga_veneno = 0; if (debugLog) fprintf(debugLog, "[%lld] VENENO_CD reset\n", tick_atual); break;
            case EVENTO_ACELERAR_CD: recarga_acelerar = 0; if (debugLog) fprintf(debugLog, "[%lld] ACELERAR_CD reset\n", tick_atual); break;
            case EVENTO_JOGADOR_ACELERAR_FIM:
                velocidade_jogador = 10;
                sprintf(log_batalha, "Acelerar terminou.");
                break;
            case EVENTO_INIMIGO_VENENO_TICK:
                if (inimigo_ticks_veneno > 0) {
                    vida_inimigo -= 10;
                    inimigo_ticks_veneno--;
                    sprintf(log_batalha, "Veneno: -10 HP");
                    if (debugLog) fprintf(debugLog, "[%lld] Veneno tick aplicado, restos=%d\n", tick_atual, inimigo_ticks_veneno);
                    // re-agendar próximo tick se ainda houver
                    if (inimigo_ticks_veneno > 0) arvore = InserirNo(tick_atual + 1500, EVENTO_INIMIGO_VENENO_TICK, arvore);
                }
                break;
            case EVENTO_TURNO_JOGADOR_PRONTO:
                // se menu já aberto (ou jogador já está no menu), mantém estado; Chrono Trigger: pausar tempo
                if (estado_atual == JOGO_RODANDO) {
                    estado_atual = MENU_ACAO_ABERTO;
                    cursor_menu = 0;
                    sprintf(log_batalha, "Seu turno! Escolha ação.");
                }
                break;
            case EVENTO_TURNO_INIMIGO_PRONTO:
                // só processa se o jogador não estiver no menu (CT style)
                if (estado_atual == JOGO_RODANDO) {
                    arvore = executar_ia_inimigo(arvore);
                } else {
                    // se jogador em menu, adia inimigo um pouco (não perde o turno)
                    arvore = InserirNo(tick_atual + 200, EVENTO_TURNO_INIMIGO_PRONTO, arvore);
                }
                break;
            case EVENTO_INIMIGO_ATAQUE_CARREGADO: {
                int dano = 30 + (rand()%10);
                vida_jogador -= dano;
                sprintf(log_batalha, "Behemoth usa ataque carregado! %d dmg", dano);
                break;
            }
            case EVENTO_FADIGA_CD:
    recarga_fadiga = 0;
    if (debugLog) fprintf(debugLog, "[%lld] FADIGA_CD reset\n", tick_atual);
    break;

case EVENTO_INIMIGO_STUN_FIM:
    inimigo_stun = 0;
    sprintf(log_batalha, "O inimigo se recuperou do atordoamento!");
    if (debugLog) fprintf(debugLog, "[%lld] STUN_FIM do inimigo\n", tick_atual);
    break;

        }
    }

    return arvore;
}

int main() {
    initscr(); cbreak(); noecho(); nodelay(stdscr, TRUE); keypad(stdscr, TRUE); curs_set(0);
    start_color(); init_pair(1, COLOR_RED, COLOR_BLACK); init_pair(2, COLOR_GREEN, COLOR_BLACK); init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    srand((unsigned)time(NULL));
    debugLog = fopen("debug_rbt.log", "w");

    // inicializa tempo real e tick_atual (usamos tempo real ms como referência)
    long long real_start = tempo_real_ms();
    tick_atual = real_start;

    // agenda turnos iniciais
    arvore_eventos = schedule_turn_player(arvore_eventos);
    arvore_eventos = schedule_turn_enemy(arvore_eventos);

    // loop principal
    int ch;
        while (vida_jogador > 0 && vida_inimigo > 0) {
        if (estado_atual == JOGO_RODANDO) {
            tick_atual = tempo_real_ms();
        }

        arvore_eventos = processar_eventos(arvore_eventos);
        eventos_na_arvore = Tamanho(arvore_eventos);

        ch = getch();
        if (ch == 'q' || ch == 'Q') break;

        // ================= MENU DE AÇÃO =================
        if (estado_atual == MENU_ACAO_ABERTO) {
            if (ch == 'a' || ch == '\n') { // pancada
                arvore_eventos = executar_acao_jogador(arvore_eventos, 0);
                // efeito stun: inimigo perde o próximo ataque (opcional)
                // arvore_eventos = RemoverMinimo(arvore_eventos);
            } 
            else if (ch == 's' || ch == 'S') arvore_eventos = executar_acao_jogador(arvore_eventos, 1);
            else if (ch == 'm' || ch == 'M') {
                estado_atual = MENU_MAGIA_ABERTO;
                cursor_menu = 0;
            }
            else if (ch == 27) { // ESC volta ao jogo sem agir
                estado_atual = JOGO_RODANDO;
                sprintf(log_batalha, "Você hesitou...");
            }
        }

        // ================= MENU DE MAGIA =================
        else if (estado_atual == MENU_MAGIA_ABERTO) {
            if (ch == 'f' || ch == 'F') arvore_eventos = executar_magia_jogador(arvore_eventos, 0);
            if (ch == 'h' || ch == 'H') arvore_eventos = executar_magia_jogador(arvore_eventos, 1);
            if (ch == 'v' || ch == 'V') arvore_eventos = executar_magia_jogador(arvore_eventos, 2);
            if (ch == 'a' || ch == 'A') arvore_eventos = executar_magia_jogador(arvore_eventos, 3);

            // agora ESC ou 'm' voltam pro menu principal de ação
            if (ch == 27 || ch == 'm' || ch == 'M') {
                estado_atual = MENU_ACAO_ABERTO;
                sprintf(log_batalha, "Voltando ao menu de ação...");
            }
        }

        // ================= TEMPO NORMAL =================
        else if (estado_atual == JOGO_RODANDO) {
            if (ch == 'a' || ch == 'A') {
                estado_atual = MENU_ACAO_ABERTO;
                sprintf(log_batalha, "Seu turno! Escolha ação.");
            }
            if (ch == 'm' || ch == 'M') {
                estado_atual = MENU_MAGIA_ABERTO;
                sprintf(log_batalha, "Escolha uma magia.");
            }
        }

        // ================= DESENHO =================
        erase();
        desenhar_interface();
        if (estado_atual == MENU_ACAO_ABERTO) {
            mvprintw(14, 3, "> [A] Pancada (stun)   [S] Golpe Cruzado    [M] Magia    [ESC] Cancelar");
        } else if (estado_atual == MENU_MAGIA_ABERTO) {
            mvprintw(14, 3, "> [F] Fogo   [H] Cura   [V] Veneno   [A] Acelerar   [ESC/M] Voltar");
        }
        refresh();

        napms(30);
    }


    // fim
    endwin();
    if (debugLog) {
        fprintf(debugLog, "Fim. HP Jogador=%d HP Inimigo=%d\n", vida_jogador, vida_inimigo);
        fclose(debugLog);
    }

    printf("--- FIM DE JOGO ---\n");
    if (vida_jogador <= 0) printf("Voce foi derrotado!\n");
    else if (vida_inimigo <= 0) printf("Voce venceu!\n");
    else printf("Jogo fechado.\n");
    return 0;
}