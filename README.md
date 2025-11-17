# Compilador para a Linguagem Goianinha - Criado por: Flávio de Oliveira Neto

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
  - `modulo_arvore_sintatica_abstrata.h` e `modulo_arvore_sintatica_abstrata.c`: Definição e implementação da Árvore Sintática Abstrata (AST).
  - `modulo_analisador_semantico.h` e `modulo_analisador_semantico.c`: Implementação do analisador semântico.
  - `modulo_gerador_codigo.h` e `modulo_gerador_codigo.c`: Implementação do gerador de código MIPS.
  - Arquivos de teste: `expressao1Correto.txt`, `fatorialCorreto.txt`, e outros que demonstram o funcionamento e a detecção de erros.

## Como Compilar o Projeto

Para compilar o `goianinha`, siga os passos abaixo:

1.  Navegue até o diretório do analisador:

    ```bash
    cd analisador_lexer_sintatico
    ```

2.  Execute o comando `make` para compilar o projeto. Este comando irá invocar o Flex e o Bison para gerar os analisadores, compilar todos os arquivos `.c` necessários e linká-los, gerando o executável `goianinha`.

    ```bash
    make
    ```

## Como Executar e Testar

Após a compilação, você pode testar o compilador com os arquivos de exemplo.

1.  Ainda no diretório `analisador_lexer_sintatico`, execute o compilador passando um dos arquivos de teste como argumento:

    ```bash
    ./goianinha <nome_do_arquivo_de_teste>.txt
    ```

    Por exemplo:

    ```bash
    ./goianinha fatorialCorreto.txt
    ```

2.  O compilador irá processar o arquivo, exibir mensagens indicando o progresso da análise e, se não houver erros, gerar um arquivo `saida.s` com o código MIPS correspondente.

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