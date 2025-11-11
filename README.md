# Simulador de Emoções

## Alunos

| Matrícula | Nome |
|-----------------------|---------------------|
| 23/1033737 | Artur Mendonça Arruda |
| 23/1035464 | Lucas Mendonça Arruda |

## Sobre o Projeto

**Simulador de Emoções** é um jogo de terminal desenvolvido em **C**, que simula o desenvolvimento emocional de uma criança até os 18 anos.
O jogo utiliza uma **Árvore Rubro-Negra (Red-Black Tree)** para gerenciar emoções, permitindo inserção, busca e remoção de sentimentos de forma balanceada.
Cada escolha feita pelo jogador influencia o desenvolvimento da personalidade do personagem.

## História do Jogo

O jogador acompanha a vida de uma criança/adolescente, passando por eventos cotidianos que desafiam suas emoções.
As escolhas feitas em cada evento moldam a personalidade e podem levar a diferentes resultados ao final dos 18 anos.

## Sobre o Jogo

  - O jogo é composto por **18 rodadas**, correspondentes a cada ano de vida do personagem.

  - Em cada rodada, o jogador enfrenta um evento e escolhe **uma das 4 opções** disponíveis.

  - As emoções possíveis são:

      - **Felicidade**
      - **Tristeza**
      - **Medo**
      - **Raiva**

  - Cada escolha gera uma emoção com intensidade variável, que é armazenada na árvore rubro-negra.

  - Emoções dominantes podem criar efeitos em cascata, influenciando decisões futuras.

## Mecânicas

### Processamento Anual

  - **Decaimento:** Emoções com intensidade baixa (menor que 30) são removidas da árvore.
  - **Ruminação:** Emoções dominantes podem gerar novas emoções similares, reforçando sentimentos.
  - **Colapso emocional:** Se a intensidade de qualquer emoção exceder o limite (`LIMITE_COLAPSO`), o personagem sofre colapso → fim do jogo.

## Estatísticas

O jogo mantém contadores internos para monitorar operações na árvore:

  - Número de inserções
  - Número de remoções
  - Número de buscas
  - Rotações executadas (balanceamento)

## Condições de Vitória e Derrota

  - **Vitória:** O personagem chega aos 18 anos sem que nenhuma emoção dominante exceda o limite crítico.
  - **Derrota:** Uma emoção dominante atinge intensidade crítica, indicando colapso emocional.

## Eventos

  - Os eventos são ajustados por faixa etária:
      - **1-3 anos:** Birras, recusa alimentar
      - **4-7 anos:** Conflitos sociais, acidentes, medos
      - **8-10 anos:** Desempenho escolar, bullying, vício em telas, tarefas
      - **11-15 anos:** Cyberbullying, pressão social, primeiros relacionamentos
      - **16-18 anos:** Vestibular, término de namoro, desafios de autonomia

# Guia de instalação

### Dependências do projeto

  - **Sistema operacional:** Linux
  - **Compilador:** GCC (versão recomendada: 9.4 ou superior)
  - **Linguagem:** C (padrão C99 ou superior)

Para usuários Windows, é necessário:

  - Instalar o GCC (GNU Compiler Collection) ou
  - Utilizar um compilador online como o [OnlineGDB](https://www.onlinegdb.com/)

Não são necessárias bibliotecas externas, apenas o compilador C padrão.

## Como executar o projeto

1.  Clone o repositório

<!-- end list -->

```bash
git clone https://github.com/EDAII/ArvoreBalanceada_RedBlack.git
```

1.  Abra o terminal na pasta do projeto com o comando: `cd ArvoreBalanceada_RedBlack`
2.  Compile todos os arquivos juntos usando o GCC:

<!-- end list -->

```bash
gcc main.c RedBlack.c -o rodar
```

3.  Execute o programa:
    ```
    ./rodar
    ```

## Árvore Red Black

A **Árvore Rubro-Negra** (Red-Black Tree) é uma **árvore binária balanceada** que garante que operações de **inserção, busca e remoção** ocorram em complexidade de O(logn), mesmo nos piores casos.
Cada nó possui uma cor (vermelho ou preto) e regras específicas para manter a árvore balanceada. Para isso, ela garante que qualquer caminho da raiz até uma folha (nil) tenha **exatamente o mesmo número** de nós pretos.

### Função no Jogo

No Simulador de Emoções, a árvore é usada para armazenar as emoções do personagem, garantindo que:

  - Emoções com maior intensidade possam ser encontradas rapidamente;
  - Emoções menos importantes possam ser removidas facilmente;
  - A estrutura permaneça balanceada, mesmo com muitas inserções e remoções ao longo do tempo.

### Casos de Inserção

Ao inserir uma nova emoção:

1.  O nó começa como vermelho.
2.  Se o pai do nó também for vermelho (violando a regra de que "nós vermelhos não podem ter filhos vermelhos"), a árvore realiza rotações e recolorizações para manter as regras da Red-Black.
3.  Caso o pai seja preto ou seja a raiz, a inserção é direta.

### Casos de Remoção

Ao remover uma emoção:

1.  Se o nó removido for vermelho, a remoção é simples.
2.  Se for preto, sua remoção viola a "altura negra" (caminhos terão um nó preto a menos), e é preciso executar rotações ou/e recolorizações para corrigir.

Casos de remoção na Red-Black (fixup):

Irmão vermelho: troca cores com o pai e rotaciona; transforma o caso em irmão preto.

Irmão preto com filhos pretos: recolore o irmão de vermelho e sobe o problema para o pai.

Irmão preto, filho próximo preto, filho distante vermelho: rotaciona o irmão e recolore, transformando em caso 4.

Irmão preto, filho distante vermelho: troca cores entre irmão e pai, rotaciona o pai, colore filho vermelho para preto; árvore balanceada.

### Busca

  - **Busca simples**: percorre a árvore para encontrar uma emoção específica.
  - **Busca mínima/máxima**: encontra rapidamente a emoção mais fraca ou mais intensa, usada para decidir a emoção dominante ou remover emoções triviais.

### Capturas de tela

[imagem 1](assets/inicio.png)

A imagem mostra o início do jogo, explicando o jogo e mostrando as escolhas possíveis.

[imagem 2](assets/escolha_ruminacao.png)

A imagem mostra a ruminação agindo, criando várias emoções de felicidade.

[imagem 3](assets/estatisticas_escolha.png)

A imagem mostra as estatísticas gerais daquela escolha, como inserção, rotações e remoções.

[imagem 4](assets/resultado.png)

A imagem mostra uma tela de fim de jogo, mostrando as estatísticas gerais.

## Conclusão

O Simulador de Emoções utiliza a Árvore Rubro-Negra por sua alta eficiência, garantindo operações em tempo logarítmico mesmo nos piores casos.
A árvore permite inserir, buscar e remover emoções de forma rápida, mantendo a estrutura balanceada automaticamente.

No jogo, isso se aplica quando:

  - São inseridas novas emoções a cada escolha do jogador;
  - É buscada a emoção dominante para decidir o impacto emocional;
  - São removidas emoções de baixa intensidade para simular esquecimento ou superação.

### Link do Vídeo de apresentação

[Link Vídeo](https://www.google.com/search?q=)

[Link Vídeo](https://www.google.com/search?q=)

## Referências

  - **CORMEN, T. H.; LEISERSON, C. E.; RIVEST, R. L.; STEIN, C.** *Introduction to Algorithms (CLRS)*. MIT Press. O Capítulo 13 é a referência fundamental para Árvores Rubro-Negras.
  - **Wikipedia.** *Árvore rubro-negra*. Uma visão geral completa das propriedades, inserção e remoção.
  - **Programiz.** *Red-Black Tree*. Um guia com exemplos de código e explicações visuais das propriedades e operações.
  - **Materias disponibilizados pelo professor Maurício Serrano**

## Histórico de Versões

\<div align="center"\>

| Versão | Data | Descrição | Autor | Revisor | Revisão |
|-----------|---------|--------------|----------|------------|------------|
| `v1.0` | 10/11/2025 | Estruturação inicial da README | [Artur Mendonça](https://github.com/ArtyMend07) e [Lucas Mendonça](https://github.com/lucasarruda9)| | |
| `v1.1` | 10/11/2025 | Correção das seções de lógica, mecânicas e referências | [Artur Mendonça](https://github.com/ArtyMend07) e [Lucas Mendonça](https://github.com/lucasarruda9)| | |