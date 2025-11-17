#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tabela_simbolos/tabela_simbolos.h"

extern int yylineno;


/**
 * @brief Enumeração dos tipos de nós da Árvore Sintática Abstrata (AST).
 * Cobre todas as construções da gramática: declarações, comandos e expressões.
 */
typedef enum {
    NO_PROGRAMA,
    NO_BLOCO,

    NO_LISTA_DECLARACOES,
    NO_LISTA_COMANDOS,
    NO_LISTA_PARAMETROS,
    NO_LISTA_ARGUMENTOS,

    NO_DECL_FUNCAO,
    NO_DECL_VARIAVEL,
    NO_PARAMETRO,

    NO_COMANDO_SE,       
    NO_COMANDO_ENQUANTO, 
    NO_COMANDO_ATRIBUICAO, 
    NO_COMANDO_LEIA,
    NO_COMANDO_ESCREVA,
    NO_COMANDO_RETORNE,
    NO_COMANDO_NOVALINHA,
    NO_CHAMADA_FUNCAO,

    NO_OP_SOMA,     
    NO_OP_SUB,      
    NO_OP_MULT,     
    NO_OP_DIV,      
    NO_OP_IGUAL,    
    NO_OP_DIFERENTE, 
    NO_OP_MENOR,     
    NO_OP_MAIOR,     
    NO_OP_MENOR_IGUAL, 
    NO_OP_MAIOR_IGUAL, 
    NO_OP_E,         
    NO_OP_OU,        
    NO_OP_NEGACAO,   
    NO_OP_UNARIO_MENOS, 

    NO_ID,
    NO_LITERAL_INT,
    NO_LITERAL_CAR,
    NO_LITERAL_STRING

} TipoNoAST;

/**
 * @brief Estrutura principal de um nó da Árvore Sintática Abstrata.
 * Esta estrutura é flexível para representar todos os tipos de nós.
 */
typedef struct NoAST {
    TipoNoAST tipo_no; 
    int linha;         

    struct NoAST *filho1;
    struct NoAST *filho2;
    struct NoAST *filho3;

    struct NoAST *proximo; 

    char *lexema; 
    int ival;     
    char cval;    

    TipoDado tipo_dado_computado; 
    struct EntradaTabela *entrada_tabela; 

} NoAST;

/**
 * @brief Cria um nó principal da AST (com filhos).
 * @param tipo O tipo do nó (TipoNoAST).
 * @param filho1 O primeiro filho (pode ser NULL).
 * @param filho2 O segundo filho (pode ser NULL).
 * @param filho3 O terceiro filho (pode ser NULL).
 * @return Um ponteiro para o novo NoAST alocado.
 */
NoAST *criar_no(TipoNoAST tipo, NoAST *filho1, NoAST *filho2, NoAST *filho3);

/**
 * @brief Cria um nó folha do tipo ID.
 * @param lexema O identificador (será copiado).
 * @return Um ponteiro para o novo NoAST alocado.
 */
NoAST *criar_no_id(char *lexema);

/**
 * @brief Cria um nó folha do tipo INT_LITERAL.
 * @param valor O valor inteiro.
 * @return Um ponteiro para o novo NoAST alocado.
 */
NoAST *criar_no_int(int valor);

/**
 * @brief Cria um nó folha do tipo CHAR_LITERAL.
 * @param valor O valor caractere.
 * @return Um ponteiro para o novo NoAST alocado.
 */
NoAST *criar_no_car(char valor);

/**
 * @brief Cria um nó folha do tipo STRING_LITERAL.
 * @param str O texto da string (será copiado).
 * @return Um ponteiro para o novo NoAST alocado.
 */
NoAST *criar_no_string(char *str);

/**
 * @brief Adiciona um nó ao final de uma lista encadeada (via ponteiro 'proximo').
 * @param lista A lista existente (pode ser NULL).
 * @param no_para_adicionar O nó a ser adicionado.
 * @return A cabeça da lista (que pode ser o novo nó, se a lista estava vazia).
 */
NoAST *adicionar_a_lista(NoAST *lista, NoAST *no_para_adicionar);

/**
 * @brief Libera recursivamente toda a memória alocada para a AST.
 * @param raiz A raiz da árvore (ou sub-árvore) a ser liberada.
 */
void liberar_ast(NoAST *raiz);


#endif