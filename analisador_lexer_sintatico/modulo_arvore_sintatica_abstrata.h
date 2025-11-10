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
    // Nós de programa e bloco
    NO_PROGRAMA,
    NO_BLOCO,

    // Nós de lista (encadeados com 'proximo')
    NO_LISTA_DECLARACOES,
    NO_LISTA_COMANDOS,
    NO_LISTA_PARAMETROS,
    NO_LISTA_ARGUMENTOS,

    // Nós de declaração
    NO_DECL_FUNCAO,
    NO_DECL_VARIAVEL,
    NO_PARAMETRO,

    // Nós de comando
    NO_COMANDO_SE,       // if (filho1) { filho2 } else { filho3 }
    NO_COMANDO_ENQUANTO, // while (filho1) { filho2 }
    NO_COMANDO_ATRIBUICAO, // filho1 (ID) = filho2 (expressao)
    NO_COMANDO_LEIA,
    NO_COMANDO_ESCREVA,
    NO_COMANDO_RETORNE,
    NO_COMANDO_NOVALINHA,
    NO_CHAMADA_FUNCAO,

    // Nós de expressão (operadores matemáticos e lógicos)
    NO_OP_SOMA,     // +
    NO_OP_SUB,      // -
    NO_OP_MULT,     // *
    NO_OP_DIV,      // /
    NO_OP_IGUAL,    // ==
    NO_OP_DIFERENTE, // !=
    NO_OP_MENOR,     // <
    NO_OP_MAIOR,     // >
    NO_OP_MENOR_IGUAL, // <=
    NO_OP_MAIOR_IGUAL, // >=
    NO_OP_E,         // &&
    NO_OP_OU,        // ||
    NO_OP_NEGACAO,   // !
    NO_OP_UNARIO_MENOS, // - (ex: -5)

    // Nós folha (terminais)
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
    //Informações gerais
    TipoNoAST tipo_no; // O tipo do nó (do enum acima)
    int linha;         // A linha do código fonte

    //Filhos da árvore
    // Usados para diferentes propósitos dependendo do tipo_no:
        // Exemplo: NO_COMANDO_SE: filho1=condição, filho2=bloco Then, filho3=bloco Else
        // Exemplo: NO_OP_SOMA:    filho1=lado esquerdo, filho2=lado direito
    struct NoAST *filho1;
    struct NoAST *filho2;
    struct NoAST *filho3;

    //Ponteiro para listas (usado para encadear nós em listas)
    struct NoAST *proximo; 

    //Armazenamento de valores (nós folha)
    char *lexema; // Para NO_ID, NO_LITERAL_STRING
    int ival;     // Para NO_LITERAL_INT
    char cval;    // Para NO_LITERAL_CAR

    //Informação semântica (preenchido durante a análise semântica)
    TipoDado tipo_dado_computado; // Tipo da expressão (int, car)
    struct EntradaTabela *entrada_tabela; // Ponteiro para a entrada na tabela de símbolos

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


#endif // AST_H