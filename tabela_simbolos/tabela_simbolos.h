#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TIPO_INT,
    TIPO_CAR,
    TIPO_VOID, 
    TIPO_FUNCAO 
} TipoDado;

typedef struct ArgumentoFuncao {
    TipoDado tipo_arg;
    char *nome_arg; 
    struct ArgumentoFuncao *proximo;
} ArgumentoFuncao;

typedef struct EntradaTabela {
    char *lexema;         
    TipoDado tipo;        
    int posicao;         
    int num_argumentos;
    TipoDado tipo_retorno;
    ArgumentoFuncao *argumentos;

    struct EntradaTabela *proximo; 
} EntradaTabela;

typedef struct TabelaSimbolos {
    EntradaTabela *primeira_entrada; 
    struct TabelaSimbolos *proximo; 
} TabelaSimbolos;

extern TabelaSimbolos *pilha_tabelas;

/**
 * @brief Inicializa a pilha de tabelas de símbolos, tornando-a vazia.
 */
void iniciar_pilha_tabela_simbolos();

/**
 * @brief Cria uma nova tabela de símbolos (um novo escopo) e a empilha no topo.
 */
void criar_novo_escopo_e_empilhar();

/**
 * @brief Pesquisa um lexema (nome) na pilha de tabelas de símbolos, começando pelo escopo atual (topo) e descendo.
 * @param nome O lexema a ser pesquisado.
 * @return Um ponteiro para a EntradaTabela encontrada ou NULL se não encontrar.
 */
EntradaTabela *pesquisar_nome_na_pilha(const char *nome);

/**
 * @brief Remove a tabela de símbolos que está no topo da pilha, encerrando o escopo atual.
 */
void remover_escopo_atual();

/**
 * @brief Insere um nome de função na tabela de símbolos atual (topo da pilha).
 * @param nome_funcao O lexema da função.
 * @param tipo_retorno O tipo de retorno da função (TIPO_INT, TIPO_CAR, TIPO_VOID).
 * @param num_argumentos O número de argumentos da função.
 * @return Um ponteiro para a EntradaTabela da função.
 */
EntradaTabela *inserir_funcao_na_tabela_atual(const char *nome_funcao, TipoDado tipo_retorno, int num_argumentos);

/**
 * @brief Insere um parâmetro na lista de argumentos de uma entrada de função.
 * @param funcao_entrada A entrada da tabela de símbolos correspondente à função.
 * @param nome_parametro O nome do parâmetro.
 * @param tipo_parametro O tipo do parâmetro.
 */
void adicionar_parametro_a_funcao(EntradaTabela *funcao_entrada, const char *nome_parametro, TipoDado tipo_parametro);


/**
 * @brief Insere uma variável na tabela de símbolos atual (topo da pilha).
 * @param nome_variavel O lexema da variável.
 * @param tipo_variavel O tipo da variável (TIPO_INT, TIPO_CAR).
 * @param posicao A posição da variável na declaração.
 */
void inserir_variavel_na_tabela_atual(const char *nome_variavel, TipoDado tipo_variavel, int posicao);

/**
 * @brief Insere um parâmetro de função na tabela de símbolos atual (topo da pilha).
 * Nota: Os parâmetros também são variáveis locais no escopo da função.
 * @param nome_parametro O lexema do parâmetro.
 * @param tipo_parametro O tipo do parâmetro (TIPO_INT, TIPO_CAR).
 * @param posicao A posição do parâmetro na declaração.
 */
void inserir_parametro_na_tabela_atual(const char *nome_parametro, TipoDado tipo_parametro, int posicao);

/**
 * @brief Elimina toda a pilha de tabelas de símbolos e libera a memória alocada.
 */
void eliminar_pilha_tabela_simbolos();

/**
 * @brief Função auxiliar para imprimir o conteúdo da tabela de símbolos atual.
 */
void imprimir_tabela_atual();

#endif
