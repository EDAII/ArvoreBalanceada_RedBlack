#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RedBlack.h"

// Enumeração para os tipos de emoção gerenciados pela árvore.
typedef enum { FELICIDADE, TRISTEZA, RAIVA, MEDO } TipoEmocao;

// --- Estruturas de Dados ---

// Representa uma opção de ação disponível para o jogador.
typedef struct {
    const char* texto; // Descrição textual da ação.
    TipoEmocao emocao; // Emoção resultante associada a esta ação.
} Escolha;

// Representa um cenário de evento específico.
typedef struct {
    const char* cenario;        // Descrição do evento.
    int min_idade;              // Idade mínima aplicável.
    int max_idade;              // Idade máxima aplicável.
    Escolha* listaDeEscolhas;    // Array de escolhas possíveis para este evento.
    int numEscolhas;            // Tamanho total do array de escolhas.
    int foiUsado;               // Flag (0 ou 1) para indicar se o evento já ocorreu.
} Evento;

// --- Banco de Dados de Escolhas ---

// Contexto: Birra (1-3 anos)
Escolha escolhas_birra[] = {
    {"Tentar distrair a criança com outro objeto", FELICIDADE},
    {"Manter a calma e esperar a birra passar", FELICIDADE},
    {"Gritar para que ela pare imediatamente", RAIVA},
    {"Ceder e dar o que ela quer para evitar escândalo", TRISTEZA}, // Gera dependência
    {"Ameaçar deixá-la sozinha no local", MEDO},
    {"Pegar no colo à força e sair do local", RAIVA}
};

// Contexto: Recusa alimentar (1-4 anos)
Escolha escolhas_comida[] = {
    {"Fazer 'aviãozinho' e transformar em brincadeira", FELICIDADE},
    {"Forçar a criança a comer tudo o que está no prato", RAIVA},
    {"Deixar sem comer até a próxima refeição", TRISTEZA},
    {"Dizer que o 'bicho papão' vem se não comer", MEDO},
    {"Oferecer apenas a sobremesa para que coma algo", TRISTEZA}, 
    {"Negociar: 'mais uma colherada e pode brincar'", FELICIDADE}
};

// Contexto: Medo noturno/Pesadelo (2-5 anos)
Escolha escolhas_pesadelo[] = {
    {"Acender uma luz fraca e verificar o quarto juntos", FELICIDADE},
    {"Permitir que durma na sua cama esta noite", MEDO}, 
    {"Mandar voltar para a cama imediatamente, sem conversa", RAIVA},
    {"Ignorar o choro para 'ensinar a ser independente'", MEDO},
    {"Zombar do medo dizendo que 'é coisa de bebê'", TRISTEZA},
    {"Contar uma história para acalmar", FELICIDADE}
};

// Contexto: Acidente doméstico/Quebra de objeto (3-6 anos)
Escolha escolhas_quebra[] = {
    {"Verificar se a criança se machucou antes de tudo", FELICIDADE},
    {"Gritar imediatamente pelo prejuízo causado", RAIVA},
    {"Explicar calmamente o valor das coisas", FELICIDADE},
    {"Bater na mão da criança como punição", RAIVA},
    {"Dizer que vai descontar de futuros presentes", TRISTEZA},
    {"Ameaçar que a polícia vem buscar quem quebra coisas", MEDO}
};

// Contexto: Conflito social/Dividir brinquedos (4-7 anos)
Escolha escolhas_dividir[] = {
    {"Sugerir brincarem juntos com o mesmo brinquedo", FELICIDADE},
    {"Obrigar a entregar o brinquedo imediatamente", RAIVA},
    {"Dizer que ela é egoísta na frente dos outros", TRISTEZA},
    {"Guardar o brinquedo para acabar com a briga", TRISTEZA},
    {"Explicar como o outro amigo se sente", FELICIDADE},
    {"Ameaçar nunca mais levá-la para brincar", MEDO}
};

// Contexto: Mentira sobre tarefas (6-10 anos)
Escolha escolhas_mentira[] = {
    {"Conversar sobre confiança e porque a verdade importa", FELICIDADE},
    {"Aplicar um castigo severo sem direito a defesa", RAIVA},
    {"Gritar e chamá-la de mentirosa", RAIVA},
    {"Dizer que está muito decepcionado(a)", TRISTEZA},
    {"Ignorar, assumindo que é 'fase'", TRISTEZA},
    {"Ameaçar contar para todos na escola", MEDO}
};

// Contexto: Desempenho escolar ruim (7-10 anos)
Escolha escolhas_escola[] = {
    {"Investigar se há dificuldades de aprendizado", FELICIDADE},
    {"Contratar um professor particular e apoiar", FELICIDADE},
    {"Proibir qualquer lazer até as notas melhorarem", RAIVA},
    {"Comparar com o desempenho de colegas ou irmãos", TRISTEZA},
    {"Dizer que ela 'não terá futuro' assim", MEDO},
    {"Culpar a escola ou o professor na frente dela", RAIVA} 
};

// Contexto: Vício em telas/celular (7-10 anos)
Escolha escolhas_telas[] = {
    {"Estabelecer horários fixos de uso em conjunto", FELICIDADE},
    {"Confiscar o aparelho abruptamente", RAIVA},
    {"Usar o tempo de tela como única moeda de troca", TRISTEZA},
    {"Ignorar o excesso de uso para ter sossego", TRISTEZA},
    {"Ameaçar que o cérebro vai 'derreter'", MEDO},
    {"Propor atividades offline interessantes em família", FELICIDADE}
};

// Contexto: Recusa em fazer tarefas domésticas (8-10 anos)
Escolha escolhas_tarefas[] = {
    {"Fazer junto com ela nas primeiras vezes", FELICIDADE},
    {"Criar um quadro de recompensas por tarefas feitas", FELICIDADE},
    {"Gritar e chamar de preguiçosa", RAIVA},
    {"Fazer a tarefa por ela, desistindo de ensinar", TRISTEZA},
    {"Ameaçar tirar todas as regalias (internet, sair)", MEDO},
    {"Explicar a importância da contribuição de todos na casa", FELICIDADE}
};

// Contexto: Bullying (vítima ou autor) (6-10 anos)
Escolha escolhas_bullying[] = {
    {"Ouvir atentamente sem julgamentos iniciais", FELICIDADE},
    {"Ir imediatamente à escola fazer um escândalo", RAIVA},
    {"Dizer para ela 'revidar' na próxima vez", RAIVA},
    {"Minimizar o problema dizendo que é 'coisa de criança'", TRISTEZA},
    {"Culpar a criança por não saber se defender", TRISTEZA},
    {"Buscar ajuda profissional/psicológica se necessário", FELICIDADE}
};

// Contexto: Cyberbullying / Redes Sociais (11-15 anos)
Escolha escolhas_cyberbullying[] = {
    {"Ensinar a bloquear o agressor e denunciar", FELICIDADE},
    {"Confiscar o celular/computador por tempo indeterminado", RAIVA},
    {"Dizer para 'ignorar' e que isso 'não é nada'", TRISTEZA},
    {"Ir tirar satisfação com os pais do agressor imediatamente", RAIVA},
    {"Ameaçar seu filho(a) para que apague a conta", MEDO}, 
    {"Ouvir e oferecer apoio emocional incondicional", FELICIDADE}
};

// Contexto: Pressão Social / Vestibular (16-18 anos)
Escolha escolhas_vestibular[] = {
    {"Contratar um tutor e ajudar a montar um plano de estudos", FELICIDADE},
    {"Gritar que ele(a) não estuda o suficiente e 'vai fracassar'", RAIVA},
    {"Dizer para 'relaxar' e não se pressionar tanto (ignorar o problema)", TRISTEZA},
    {"Comparar com o primo/irmão que 'passou em 1º lugar'", TRISTEZA},
    {"Ameaçar cortar privilégios se a nota não melhorar", MEDO}, 
    {"Incentivar pausas e atividades de lazer para equilibrar", FELICIDADE}
};

// Contexto: Primeiro Relacionamento / Término (15-18 anos)
Escolha escolhas_termino[] = {
    {"Acolher, ouvir o desabafo e validar seus sentimentos", FELICIDADE},
    {"Dizer 'Eu avisei' e criticar o(a) ex-parceiro(a)", RAIVA},
    {"Falar mal do(a) ex, dizendo que 'não era bom o suficiente'", RAIVA},
    {"Minimizar, dizendo que 'é bobagem' e 'logo arranja outro(a)'", TRISTEZA},
    {"Proibir de sair ou ver amigos para 'evitar mais sofrimento'", MEDO},
    {"Sugerir que saia e 'fique com outra pessoa' para esquecer", MEDO} 
};

// Contexto: Pedido de item caro (Celular) (12-15 anos)
Escolha escolhas_celular_novo[] = {
    {"Explicar o orçamento familiar e propor um plano para ele(a) merecer", FELICIDADE},
    {"Comprar imediatamente, mesmo sem poder, para evitar frustração", TRISTEZA},
    {"Gritar que 'não dá valor ao dinheiro' e proibir de pedir novamente", RAIVA},
    {"Dizer 'não' sem explicações e ignorar o assunto", TRISTEZA},
    {"Prometer que vai dar 'se ele(a) se comportar', sem intenção de cumprir", MEDO},
    {"Sugerir que ele(a) trabalhe (ex: tarefas extras) para ganhar o dinheiro", FELICIDADE}
};

// Contexto: Mentira sobre paradeiro (13-16 anos)
Escolha escolhas_mentira_adolescente[] = {
    {"Conversar calmamente sobre os perigos e a quebra de confiança", FELICIDADE},
    {"Colocar de castigo por um mês, sem celular e sem sair", RAIVA},
    {"Gritar e acusar de ser 'mau caráter'", RAIVA},
    {"Dizer o quanto está decepcionado(a) e parar de falar com ele(a)", TRISTEZA},
    {"Ameaçar instalar um app de rastreamento no celular dele(a)", MEDO},
    {"Tentar entender *por que* ele(a) mentiu", FELICIDADE}
};

// Contexto: Recusa de tarefas (Adolescente) (16-18 anos)
Escolha escolhas_tarefas_adolescente[] = {
    {"Rever as responsabilidades em família e o que é esperado dele(a)", FELICIDADE},
    {"Cortar a internet/mesada até que as tarefas sejam feitas", RAIVA},
    {"Fazer as tarefas por ele(a), reclamando em voz alta", TRISTEZA},
    {"Dizer que ele(a) é 'preguiçoso(a)' e 'não faz nada certo'", TRISTEZA},
    {"Ameaçar expulsá-lo(a) de casa 'quando fizer 18 anos'", MEDO},
    {"Sentar e negociar um novo 'acordo' de tarefas", FELICIDADE}
};


// --- Banco de Dados de Eventos (COM CORREÇÕES) ---
Evento bancoDeEventos[] = {
    // Faixa Etária: 1-3 anos
    { "A criança se joga no chão do mercado gritando porque quer um doce.", 1, 3, escolhas_birra, 6, 0 },
    { "A criança se recusa a abrir a boca para comer o almoço.", 1, 3, escolhas_comida, 6, 0 },
    { "A criança chora inconsolavelmente quando você tenta sair para trabalhar.", 1, 3, escolhas_birra, 6, 0 },

    // Faixa Etária: 2-5 anos
    { "A criança acorda gritando no meio da noite dizendo que viu um monstro.", 2, 5, escolhas_pesadelo, 6, 0 },
    { "A criança quebrou um vaso caro enquanto corria pela sala.", 3, 6, escolhas_quebra, 6, 0 },
    { "A criança bateu no colega que pegou seu brinquedo.", 3, 6, escolhas_dividir, 6, 0 },

    // Faixa Etária: 4-7 anos
    { "A criança não quer emprestar seu brinquedo novo para o primo.", 4, 7, escolhas_dividir, 6, 0 },
    { "A criança desenhou na parede recém-pintada da sala.", 4, 7, escolhas_quebra, 6, 0 },
    { "A criança chora porque não quer ir para a escola/creche.", 4, 7, escolhas_birra, 6, 0 }, 

    // Faixa Etária: 6-10 anos
    { "A criança disse que já fez a lição de casa, mas você descobriu que não.", 6, 10, escolhas_mentira, 6, 0 },
    { "A professora enviou um bilhete sobre comportamento agressivo na aula.", 6, 10, escolhas_bullying, 6, 0 },
    { "A criança trouxe um boletim com várias notas vermelhas.", 7, 10, escolhas_escola, 6, 0 },
    { "A criança se recusa a desligar o videogame para vir jantar.", 7, 10, escolhas_telas, 6, 0 },
    { "A criança chega da escola chorando dizendo que ninguém gosta dela.", 7, 10, escolhas_bullying, 6, 0 },
    { "A criança não arrumou o quarto mesmo após você pedir três vezes.", 8, 10, escolhas_tarefas, 6, 0 },
    
    // Faixa Etária: 11-15 anos (Início da Adolescência)
    { "O(A) adolescente vê comentários maldosos sobre sua foto no Instagram.", 11, 15, escolhas_cyberbullying, 6, 0 },
    { "O primeiro 'crush' do(a) adolescente o(a) rejeita na frente de todos.", 12, 15, escolhas_termino, 6, 0 },
    { "O(A) adolescente quer desesperadamente um celular novo que você não pode pagar.", 12, 15, escolhas_celular_novo, 6, 0 }, 
    { "Você descobre que o(a) adolescente mentiu sobre onde estava.", 13, 16, escolhas_mentira_adolescente, 6, 0 },
    
    // Faixa Etária: 16-18 anos (Adolescência Final)
    { "A pressão para o vestibular está causando crises de ansiedade no(a) seu/sua filho(a).", 16, 18, escolhas_vestibular, 6, 0 },
    { "O primeiro namoro sério do(a) adolescente termina abruptamente.", 16, 18, escolhas_termino, 6, 0 },
    { "Seu/Sua filho(a) é pego(a) 'colando' em uma prova importante.", 16, 18, escolhas_mentira_adolescente, 6, 0 },
    { "O(A) adolescente se recusa a ajudar nas tarefas, dizendo que 'precisa estudar'.", 16, 18, escolhas_tarefas_adolescente, 6, 0 }
};

#define NUM_EVENTOS (sizeof(bancoDeEventos) / sizeof(Evento))

// --- Variáveis Globais ---
No *raiz = NULL;
int idade = 0;

// --- Funções Auxiliares ---

int gerar_intensidade(int min, int max) {
    return min + rand() % (max - min + 1);
}

void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Embaralha um array de inteiros de tamanho 'n'
void shuffle_indices(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// === NOVA FUNÇÃO DE TEXTO (NARRATIVA) ===
// Mapeia o enum para uma string para textos do jogo
const char* getNomeEmocao(TipoEmocao tipo) {
    switch (tipo) {
        case FELICIDADE: return "Felicidade";
        case TRISTEZA:   return "Tristeza";
        case RAIVA:      return "Raiva";
        case MEDO:       return "Medo";
        default:         return "Desconhecida";
    }
}
// ========================================

// --- Lógica Principal do Jogo ---

// Adiciona uma emoção à árvore, garantindo intensidade única.
void adicionar_emocao(TipoEmocao tipo, int intensidade) {
    long long valor_unico = (long long)intensidade;

    // Busca por uma chave (intensidade) vaga
    while (BuscarNo(raiz, valor_unico) != NIL) {
        valor_unico++; 
    }

    // O printf agora usa a função de nome para clareza
    printf("-> Emoção gerada: [%s] | Intensidade: %lld\n", getNomeEmocao(tipo), valor_unico);
    No *novoNo = CriarNo(valor_unico, tipo);
    RB_insert(&raiz, novoNo);
}

// Retorna o nó com a maior intensidade (o mais à direita)
No* emocao_dominante() {
    return BuscarMaximo(raiz);
}

// Mecânica de Remoção em Massa (Texto melhorado)
void ProcessarDecaimento(No **T) {
    printf("\n-> Processando o que passou no último ano...\n");
    int removidas = 0;
    const int INTENSIDADE_TRIVIAL = 30; // Limite de remoção

    while (1) {
        No *fraco = Tree_minimum(*T);
        if (fraco == NIL || fraco->valor >= INTENSIDADE_TRIVIAL) {
            break; 
        }
        RB_delete(T, fraco);
        removidas++;
    }

    if (removidas > 0) {
        printf("-> %d sentimentos triviais foram esquecidos.\n", removidas);
    } else {
        printf("-> Nenhuma emoção trivial foi esquecida.\n");
    }
}

// Mecânica de "Explosão" de Interações (Texto melhorado)
void ProcessarRuminacao(No* dominante) {
    if (dominante == NULL || dominante->valor < 30) {
        return;
    }

    int bonus_reforcos = (dominante->valor / 10); 
    int num_reforcos = 5 + bonus_reforcos + (rand() % 5);

    // Texto de "Jogo" ao invés de "Técnico"
    printf("\n-> Seu/Sua filho(a) está ruminando sobre [ %s ] (Int: %lld)...\n", 
           getNomeEmocao(dominante->chave), dominante->valor);
    printf("-> Isso gerou %d novos sentimentos de [ %s ].\n", 
           num_reforcos, getNomeEmocao(dominante->chave));
    
    for (int i = 1; i <= num_reforcos; i++) {
        int nova_intensidade = dominante->valor - i;
        
        if (nova_intensidade < 30) {
            nova_intensidade = 30 + (rand() % 5);
        }
        
        printf("   -> Reforço: ");
        adicionar_emocao(dominante->chave, nova_intensidade);
    }
}

// Executa o ciclo de um evento anual.
void evento(int idade) {
    printf("\n================ [ IDADE: %d ANOS ] ================\n", idade);

    // 1. Filtragem de eventos aplicáveis
    Evento* eventosValidos[NUM_EVENTOS];
    int numEventosValidos = 0;

    for (int i = 0; i < NUM_EVENTOS; i++) {
        if (idade >= bancoDeEventos[i].min_idade && 
            idade <= bancoDeEventos[i].max_idade && 
            !bancoDeEventos[i].foiUsado) 
        {
            eventosValidos[numEventosValidos] = &bancoDeEventos[i];
            numEventosValidos++;
        }
    }

    if (numEventosValidos == 0) {
        printf("Um ano relativamente tranquilo. Nenhum evento marcante.\n");
        return;
    }

    // 2. Seleção aleatória de um evento
    int indiceEscolhido = rand() % numEventosValidos;
    Evento* eventoAtual = eventosValidos[indiceEscolhido];
    eventoAtual->foiUsado = 1; // Marca como usado

    printf("--- EVENTO ---\n%s\n", eventoAtual->cenario);
    printf("\n--- O que você faz? ---\n"); 

    // 3. Mecânica de Viés Emocional
    No* dominante_previo = emocao_dominante(); 
    TipoEmocao emo_dominante = -1; 
    
    if (dominante_previo != NULL) { 
        emo_dominante = dominante_previo->chave; 
    }

    int n = eventoAtual->numEscolhas;
    int* indices = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) indices[i] = i;

    shuffle_indices(indices, n); 

    // 4. Apresentação das 4 opções com viés
    Escolha escolhasApresentadas[4];
    int numApresentadas = 0;
    int tentativas[n]; 
    for(int i=0; i<n; i++) tentativas[i] = 0;

    if (dominante_previo != NULL) { 
        for (int i = 0; i < n && numApresentadas < 2; i++) {
            int idx = indices[i];
            if (eventoAtual->listaDeEscolhas[idx].emocao == emo_dominante) {
                escolhasApresentadas[numApresentadas] = eventoAtual->listaDeEscolhas[idx];
                tentativas[idx] = 1; 
                numApresentadas++;
            }
        }
    }

    for (int i = 0; i < n && numApresentadas < 4; i++) {
        int idx = indices[i];
        if (tentativas[idx] == 0) { 
            escolhasApresentadas[numApresentadas] = eventoAtual->listaDeEscolhas[idx];
            tentativas[idx] = 1;
            numApresentadas++;
        }
    }

    for (int i = 0; i < numApresentadas; i++) {
        printf("  [%d] %s\n", i + 1, escolhasApresentadas[i].texto);
    }
    free(indices); 

    // 5. Leitura e validação da entrada do usuário
    int escolhaUsuario = -1;
    while (escolhaUsuario < 1 || escolhaUsuario > numApresentadas) {
        printf("\n> Digite sua escolha (1-%d): ", numApresentadas); 
        
        if (scanf("%d", &escolhaUsuario) != 1) {
            escolhaUsuario = -1;
        }
        limpar_buffer_entrada();
        if (escolhaUsuario < 1 || escolhaUsuario > numApresentadas) {
            printf("Entrada inválida. Tente novamente.\n");
        }
    }

    // 6. Processamento da consequência emocional (Inserção Múltipla)
    Escolha escolhida = escolhasApresentadas[escolhaUsuario - 1];
    int intensidade = gerar_intensidade(20, 40); 

    printf("\n-> Emoção Principal: ");
    adicionar_emocao(escolhida.emocao, intensidade);

    if (escolhida.emocao == RAIVA || escolhida.emocao == MEDO) {
        printf("-> Esta ação teve consequências complexas...\n");
        
        printf("   -> Sub-emoção (Culpa/Tristeza): ");
        adicionar_emocao(TRISTEZA, gerar_intensidade(20, 25)); 
        
        printf("   -> Sub-emoção (Medo/Ansiedade): ");
        adicionar_emocao(MEDO, gerar_intensidade(20, 24));    
    }

    // 7. Verificação de evento crítico aleatório (10% de chance)
    if (rand() % 10 == 0) {
        printf("\n[! CRÍTICO !] A situação escalou inesperadamente!\n");
        int intensidadeExtrema = gerar_intensidade(130, 200);
        adicionar_emocao(escolhida.emocao, intensidadeExtrema); 
    }
}

// --- Main ---
int main() {
    srand((unsigned int)time(NULL));

    InicializarNIL();
    raiz = NIL;

    for (int i = 0; i < NUM_EVENTOS; i++) {
        bancoDeEventos[i].foiUsado = 0;
    }

    printf("=== SIMULADOR DE DESENVOLVIMENTO (1-18 ANOS) ===\n");
    printf("Parâmetros Emocionais: 0=FELICIDADE, 1=TRISTEZA, 2=RAIVA, 3=MEDO\n");
    printf("Objetivo: Chegar aos 18 anos sem causar traumas extremos (intensidade > 150).\n");
    
    const int LIMITE_COLAPSO = 150; 

    for (idade = 1; idade <= 18; idade++) {
        
        // FASE DE REMOÇÃO (Início do ano)
        if (idade > 1) { 
            ProcessarDecaimento(&raiz);
        }

        // FASE DE INSERÇÃO (Evento)
        evento(idade);

        No *dominante_pos_evento = emocao_dominante();
        
        // FASE DE "EXPLOSÃO" (Pós-evento, Inserção em Massa)
        if (dominante_pos_evento != NULL) {
             ProcessarRuminacao(dominante_pos_evento);
        }
        
        // FASE DE VERIFICAÇÃO (Fim do ano)
        No *dominante_final = emocao_dominante(); 
        if (dominante_final != NULL) {
            printf("\nEstado Emocional Dominante no fim do Ano %d: [%s] | Intensidade: %lld\n", 
                   idade, getNomeEmocao(dominante_final->chave), dominante_final->valor);

            // === NOVO: EXIBIÇÃO DOS CONTADORES ===
            printf("--- [Estatísticas da Árvore (Total)] ---\n");
            printf("   Inserções: %-7lld | Remoções: %-7lld\n", g_count_insert, g_count_remove);
            printf("   Buscas (Simples): %-7lld | Buscas (Min/Max): %-7lld\n", g_count_search, g_count_search_min_max);
            printf("   Rotações (Balanceamento): %-7lld\n", g_count_rotations);
            // ======================================

            if (dominante_final->valor > LIMITE_COLAPSO) {
                printf("\n[FIM DE JOGO] Colapso emocional detectado. Intensidade crítica (%lld) atingida.\n", dominante_final->valor);
                break;
            }
        } else {
             printf("\nEstado Emocional Dominante: [Nenhum]\n");
        }
    }

    printf("\n================ [ RESULTADO FINAL ] ================\n");
    No *final = emocao_dominante(); 
    if (final != NULL && final->valor <= LIMITE_COLAPSO) {
        printf("Ciclo de 18 anos concluído com sucesso.\n");
        printf("Emoção predominante na formação da personalidade: [%s] (Intensidade %lld)\n", 
               getNomeEmocao(final->chave), final->valor);
    } else if (final == NULL) {
        printf("Desenvolvimento concluído sem registros emocionais significativos.\n");
    } else {
        printf("O desenvolvimento terminou com um estado emocional instável.\n");
    }

    DesalocarArvore(raiz);
    free(NIL);

    return 0;
}