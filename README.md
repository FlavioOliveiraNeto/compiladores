# Compilador para a Linguagem Goianinha

Este projeto é um compilador para uma linguagem de programação simples e didática chamada "Goianinha". O compilador foi desenvolvido em C, utilizando as ferramentas Flex para a análise léxica e Bison para a análise sintática.

## Estrutura do Projeto

O projeto está organizado em dois diretórios principais:

- `tabela_simbolos/`: Este diretório é dedicado à implementação da tabela de símbolos, um componente crucial para a análise semântica que gerencia identificadores (variáveis, funções, etc.) e seus respectivos escopos.

  - `tabela_simbolos.h`: Arquivo de cabeçalho que define as estruturas de dados centrais, como `EntradaTabela` (para armazenar informações sobre cada identificador), `TabelaSimbolos` (para representar um escopo) e o `enum` `TipoDado`. Ele também declara os protótipos de todas as funções públicas para manipulação da tabela.
  - `tabela_simbolos.c`: Contém a implementação das funções para gerenciar a tabela de símbolos. A lógica principal utiliza uma pilha de tabelas para lidar com escopos aninhados, permitindo criar e destruir escopos, bem como inserir e buscar símbolos.
  - `main_tabela_simbolos.c`: Um programa de teste (`main`) autônomo, criado para verificar a corretude da tabela de símbolos. Ele executa operações de criação de escopo, inserção de símbolos e busca, imprimindo os resultados para validação.
  - `Makefile`: Automatiza a compilação do programa de teste, ligando `main_tabela_simbolos.c` e `tabela_simbolos.c` para gerar o executável `test_tabela_simbolos`.

- `analisador_lexer_sintatico/`: Contém o núcleo do compilador, incluindo:
  - `goianinha.l`: A definição do analisador léxico (Flex).
  - `goianinha.y`: A definição do analisador sintático e da gramática (Bison).
  - `main_compiler.c`: O programa principal que orquestra a compilação.
  - `Makefile`: Para automatizar o processo de compilação do `goianinha`.
  - `arquivo_teste.g`: Um arquivo de exemplo com código-fonte na linguagem Goianinha.

## Como Compilar o Projeto

Para compilar o `goianinha`, siga os passos abaixo:

1.  Navegue até o diretório da tabela de símbolos:

    ```bash
    cd tabela_simbolos
    ```

2.  Execute o comando `make` para compilar o projeto e gerar os arquivos: 'main_tabela_simbolos.o', ''tabela_simbolos.o' e 'test_tabela_simbolos'.

    ```bash
    make
    ```

3.  Navegue até o diretório do analisador:

    ```bash
    cd analisador_lexer_sintatico
    ```

4.  Execute o comando `make` para compilar o projeto. Este comando irá invocar o Flex e o Bison para gerar os analisadores, compilar todos os arquivos `.c` necessários e linká-los, gerando o executável `goianinha`.

    ```bash
    make
    ```

## Como Executar e Testar

Após a compilação, você pode testar o compilador com o arquivo de exemplo `arquivo_teste.g`.

1.  Ainda no diretório `analisador_lexer_sintatico`, execute o compilador passando o arquivo `arquivo_teste.g` como argumento:

    ```bash
    ./goianinha arquivo_teste.g
    ```

2.  O compilador irá processar o arquivo e exibir mensagens indicando o progresso da análise sintática e semântica.

## Testando a Tabela de Símbolos Separadamente

É possível compilar e testar a implementação da tabela de símbolos de forma isolada.

1.  Navegue até o diretório `tabela_simbolos`:

    ```bash
    cd ../tabela_simbolos
    ```

2.  Compile o programa de teste:

    ```bash
    make
    ```

3.  Execute o teste:
    ```bash
    ./test_tabela_simbolos
    ```
    Isso executará uma série de operações na tabela de símbolos e imprimirá os resultados, permitindo verificar a sua corretude.

## Limpeza

Para remover todos os arquivos gerados durante a compilação (arquivos objeto e executáveis) em ambos os diretórios, você pode usar o comando `make clean` em cada um deles.

- No diretório `analisador_lexer_sintatico`:
  ```bash
  make clean
  ```
- No diretório `tabela_simbolos`:
  ```bash
  make clean
  ```
