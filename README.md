# Compilador para a Linguagem Goianinha

**Autor:** Flávio de Oliveira Neto

## 1. Introdução

Este projeto apresenta um compilador completo para a **Goianinha**, uma linguagem de programação imperativa e didática, projetada para ser simples e funcional. O compilador processa o código-fonte em Goianinha, passando por todas as fases clássicas da compilação — análise léxica, sintática, semântica e geração de código — para produzir um código assembly em MIPS, que pode ser executado em simuladores como o SPIM ou o MARS.

O objetivo deste projeto é demonstrar o entendimento prático e teórico da construção de compiladores, aplicando conceitos fundamentais como o uso de geradores de analisadores (Flex e Bison), a criação de estruturas de dados intermediárias (Tabela de Símbolos e Árvore Sintática Abstrata) e a tradução de uma linguagem de alto nível para uma linguagem de baixo nível.

## 2. Arquitetura do Compilador

O compilador é construído de forma modular, onde cada componente é responsável por uma fase específica do processo de compilação. A interação entre os módulos ocorre da seguinte forma:

1.  **Analisador Léxico (`goianinha.l`)**: Lê o código-fonte e o divide em uma sequência de *tokens*.
2.  **Analisador Sintático (`goianinha.y`)**: Recebe os *tokens* do analisador léxico, verifica se a sequência obedece à gramática da linguagem e constrói a **Árvore Sintática Abstrata (AST)**.
3.  **Analisador Semântico (`modulo_analisador_semantico.c`)**: Percorre a AST, utilizando a **Tabela de Símbolos** para verificar a corretude semântica do programa (ex: declaração de variáveis, compatibilidade de tipos).
4.  **Gerador de Código (`modulo_gerador_codigo.c`)**: Se a análise semântica for bem-sucedida, este módulo percorre a AST (agora enriquecida com informações de tipo e escopo) e gera o código assembly MIPS equivalente.

O fluxo de dados pode ser visualizado da seguinte maneira:

```
Código-Fonte Goianinha -> [Analisador Léxico] -> Tokens -> [Analisador Sintático] -> AST -> [Analisador Semântico] -> AST Validada -> [Gerador de Código] -> Código Assembly MIPS
```

## 3. Detalhamento dos Componentes

### 3.1. Tabela de Símbolos (`tabela_simbolos/`)

A Tabela de Símbolos é uma das estruturas de dados mais importantes do compilador, responsável por armazenar informações sobre todos os identificadores (nomes de variáveis, funções, etc.) do programa.

- **Gerenciamento de Escopo**: A implementação utiliza uma **pilha de tabelas**, onde cada tabela representa um escopo (global, de função ou de bloco). Ao entrar em um novo escopo (ex: uma função ou um bloco `{...}`), uma nova tabela é empilhada. Ao sair, ela é desempilhada. Isso garante que as regras de visibilidade de variáveis sejam respeitadas.
- **Estruturas de Dados**:
  - `EntradaTabela`: Armazena o lexema (nome), o tipo de dado, a categoria (variável ou função) e, para funções, informações sobre os parâmetros.
  - `TabelaSimbolos`: Representa um escopo, contendo uma lista ligada de `EntradaTabela`.
- **Funcionalidades**:
  - `criar_novo_escopo_e_empilhar()`: Cria e empilha um novo escopo.
  - `remover_escopo_atual()`: Destrói o escopo atual e libera a memória.
  - `inserir_variavel_na_tabela_atual()` / `inserir_funcao_na_tabela_atual()`: Adiciona novos símbolos ao escopo atual.
  - `pesquisar_nome_na_pilha()`: Busca por um identificador, começando do escopo atual e descendo pela pilha, o que implementa a regra de escopo da linguagem.

### 3.2. Analisador Léxico (`analisador_lexer_sintatico/goianinha.l`)

Implementado com o **Flex**, o analisador léxico (ou *scanner*) é a porta de entrada do compilador. Sua função é ler o arquivo de código-fonte e converter a sequência de caracteres em uma sequência de *tokens*, que são as unidades mínimas de significado da linguagem.

- **Reconhecimento de Padrões**: Utiliza expressões regulares para identificar:
  - **Palavras-chave**: `programa`, `int`, `se`, `enquanto`, etc.
  - **Identificadores**: Nomes de variáveis e funções (ex: `minhaVariavel`, `soma`).
  - **Literais**: Números (`123`), caracteres (`'a'`) e strings (`"ola"`).
  - **Operadores**: Aritméticos (`+`, `-`), relacionais (`==`, `>`) e lógicos (`&&`, `!`).
  - **Símbolos**: Parênteses, chaves, ponto e vírgula, etc.
- **Tratamento de Espaços e Comentários**: Ignora espaços em branco, tabulações e comentários (`// ...` e `/* ... */`).
- **Comunicação com o Parser**: Ao reconhecer um padrão, ele retorna um código de *token* para o analisador sintático (Bison) e, quando necessário, o valor associado ao *token* (ex: o nome de um `ID` ou o valor de um `INT_LITERAL`) através da variável `yylval`.

### 3.3. Analisador Sintático e a AST (`analisador_lexer_sintatico/goianinha.y`)

Implementado com o **Bison**, o analisador sintático (ou *parser*) tem duas funções principais:

1.  **Análise da Estrutura**: Verifica se a sequência de *tokens* fornecida pelo léxico está em conformidade com a **gramática livre de contexto** da linguagem Goianinha. Se uma regra sintática é violada (ex: um `se` sem `entao`), ele reporta um erro.
2.  **Construção da Árvore Sintática Abstrata (AST)**: Durante a análise, o parser constrói uma AST, que é uma representação hierárquica do código-fonte, muito mais adequada para as fases seguintes do que o texto plano.

- **A Gramática**: A gramática é definida em um formato BNF-like. Ela especifica como as construções da linguagem (declarações, comandos, expressões) podem ser formadas. Regras de precedência e associatividade de operadores são definidas para resolver ambiguidades (ex: `a + b * c`).
- **A AST (`modulo_arvore_sintatica_abstrata.c`)**:
  - Cada nó na árvore (`NoAST`) representa uma construção do código. Por exemplo, um comando `se` é um nó com três filhos: a condição, o bloco `entao` e o bloco `senao`.
  - A árvore captura a estrutura lógica do programa, descartando detalhes puramente sintáticos como parênteses e ponto e vírgula.
  - A raiz da árvore (`raiz_ast`) representa o programa como um todo.

### 3.4. Analisador Semântico (`analisador_lexer_sintatico/modulo_analisador_semantico.c`)

Após a validação da sintaxe, o analisador semântico percorre a AST para verificar se o código, embora sintaticamente correto, faz sentido do ponto de vista lógico e das regras de tipo da linguagem.

- **Análise de Escopo**:
  - Verifica se cada variável ou função utilizada foi previamente declarada.
  - Garante que não há múltiplas declarações do mesmo identificador no mesmo escopo.
- **Checagem de Tipos**:
  - Valida se os tipos em operações são compatíveis. Por exemplo, não permite somar um `int` com uma `string`.
  - Verifica se o tipo de valor retornado por uma função (`retorne ...`) corresponde ao tipo de retorno declarado para ela.
  - Assegura que os argumentos passados em uma chamada de função correspondem em número e tipo aos parâmetros declarados.
- **Enriquecimento da AST**: Durante a travessia, o analisador anota os nós da AST com informações de tipo e ponteiros para as entradas correspondentes na Tabela de Símbolos. Essa informação é crucial para o gerador de código.

### 3.5. Gerador de Código (`analisador_lexer_sintatico/modulo_gerador_codigo.c`)

A fase final do compilador. O gerador de código percorre a AST, já validada e anotada pelo analisador semântico, e traduz cada construção da linguagem Goianinha para uma sequência de instruções em **assembly MIPS**.

- **Convenções de Chamada e Gerenciamento da Pilha**:
  - **Prólogo e Epílogo**: Cada função gerada possui um prólogo (para salvar registradores e alocar espaço para variáveis locais na pilha) e um epílogo (para restaurar a pilha e retornar ao chamador).
  - **Passagem de Argumentos**: Os argumentos para funções são passados através da pilha.
  - **Variáveis Locais**: São acessadas através de um deslocamento (*offset*) a partir do registrador de ponteiro de quadro (`$fp`).
- **Tradução das Construções**:
  - **Expressões Aritméticas**: São traduzidas para operações MIPS como `add`, `sub`, `mult`, `div`. O resultado de expressões intermediárias é salvo temporariamente na pilha.
  - **Comandos de Controle de Fluxo**:
    - `se-entao-senao`: Implementado com saltos condicionais (`beq`, `bne`) e incondicionais (`j`).
    - `enquanto`: Traduzido para um loop com um teste no início e um salto para o final se a condição for falsa.
  - **Chamadas de Função**: Utiliza a instrução `jal` (jump and link) para saltar para a função e salvar o endereço de retorno.
- **Saída**: O resultado é um arquivo de texto (por padrão, `saida.s`) contendo o código MIPS, pronto para ser executado em um simulador.

## 4. A Linguagem Goianinha

Goianinha é uma linguagem simples com as seguintes características:

- **Tipos de Dados**: `int`, `car`, `void`.
- **Estrutura**: Todo programa começa com declarações globais, seguidas por `programa { ... }` que funciona como a função `main`.
- **Declarações**: Suporta declaração de variáveis e funções.
- **Comandos**:
  - Atribuição (`=`).
  - Entrada e Saída (`leia`, `escreva`, `novalinha`).
  - Controle de fluxo (`se-entao-senao`, `enquanto-execute`).
  - Retorno de funções (`retorne`).
- **Expressões**: Aritméticas, relacionais e lógicas.

## 5. Como Compilar e Executar

### Compilando o Compilador

1.  Navegue até o diretório principal do compilador:
    ```bash
    cd analisador_lexer_sintatico
    ```
2.  Execute o `make` para compilar todas as partes e gerar o executável `goianinha`:
    ```bash
    make
    ```

### Executando o Compilador

1.  Com o compilador `goianinha` gerado, execute-o passando um arquivo-fonte como argumento:
    ```bash
    ./goianinha ../exemplos/fatorialCorreto.txt
    ```
2.  O compilador exibirá o progresso da compilação. Se não houver erros, um arquivo `saida.s` será criado no mesmo diretório.

### Executando o Código Gerado (com SPIM)

1.  Abra o simulador SPIM (ou `qtsqim`).
2.  Carregue o arquivo `saida.s` gerado.
3.  Execute o programa. A interação (entrada e saída) ocorrerá no console do simulador.

## 6. Testando o Compilador

O diretório `analisador_lexer_sintatico/` contém vários arquivos de teste (`.txt`) que podem ser usados para verificar a funcionalidade do compilador:

- **Testes de Sucesso**:
  - `fatorialCorreto.txt`: Calcula o fatorial de um número.
  - `expressao1Correto.txt`: Testa expressões aritméticas e lógicas.
- **Testes de Erro**:
  - `fatorialErroLin3NomeDeclaradoNoMesmoEscopo.txt`: Testa a detecção de variável redeclarada.
  - `fatorialErroLin4TipoRetornado.txt`: Testa a checagem de tipo de retorno.

Esses testes são essenciais para validar tanto a capacidade do compilador de gerar código correto quanto sua robustez na identificação de erros semânticos.

## 7. Limpeza do Projeto

Para remover todos os arquivos gerados pela compilação (objetos, executáveis, etc.), use o comando `make clean` nos respectivos diretórios:

```bash
# No diretório analisador_lexer_sintatico/
make clean

# No diretório tabela_simbolos/
cd ../tabela_simbolos
make clean
```
