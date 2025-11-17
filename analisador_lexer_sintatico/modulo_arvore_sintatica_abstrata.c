#include "modulo_arvore_sintatica_abstrata.h"

/**
 * @brief Aloca e inicializa um nó genérico da AST.
 */
static NoAST* alocar_no() {
    NoAST *no = (NoAST*) malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro critico: Falha ao alocar memoria para NoAST.\n");
        exit(EXIT_FAILURE);
    }
    
    no->tipo_no = NO_PROGRAMA; 
    no->linha = yylineno; 
    no->filho1 = NULL;
    no->filho2 = NULL;
    no->filho3 = NULL;
    no->proximo = NULL;
    no->lexema = NULL;
    no->ival = 0;
    no->cval = '\0';
    no->tipo_dado_computado = TIPO_VOID; 
    no->entrada_tabela = NULL;
    
    return no;
}

/**
 * @brief Cria um nó principal da AST (com filhos).
 */
NoAST *criar_no(TipoNoAST tipo, NoAST *filho1, NoAST *filho2, NoAST *filho3) {
    NoAST *no = alocar_no();
    no->tipo_no = tipo;
    no->filho1 = filho1;
    no->filho2 = filho2;
    no->filho3 = filho3;
    return no;
}

/**
 * @brief Cria um nó folha do tipo ID.
 */
NoAST *criar_no_id(char *lexema) {
    NoAST *no = alocar_no();
    no->tipo_no = NO_ID;
    no->lexema = strdup(lexema); 
    if (no->lexema == NULL) {
        fprintf(stderr, "Erro critico: Falha ao duplicar lexema para NO_ID.\n");
        exit(EXIT_FAILURE);
    }
    return no;
}

/**
 * @brief Cria um nó folha do tipo INT_LITERAL.
 */
NoAST *criar_no_int(int valor) {
    NoAST *no = alocar_no();
    no->tipo_no = NO_LITERAL_INT;
    no->ival = valor;
    return no;
}

/**
 * @brief Cria um nó folha do tipo CHAR_LITERAL.
 */
NoAST *criar_no_car(char valor) {
    NoAST *no = alocar_no();
    no->tipo_no = NO_LITERAL_CAR;
    no->cval = valor;
    return no;
}

/**
 * @brief Cria um nó folha do tipo STRING_LITERAL.
 */
NoAST *criar_no_string(char *str) {
    NoAST *no = alocar_no();
    no->tipo_no = NO_LITERAL_STRING;
    no->lexema = strdup(str);
    if (no->lexema == NULL) {
        fprintf(stderr, "Erro critico: Falha ao duplicar lexema para NO_LITERAL_STRING.\n");
        exit(EXIT_FAILURE);
    }
    return no;
}

/**
 * @brief Adiciona um nó ao final de uma lista encadeada (via ponteiro 'proximo').
 */
NoAST *adicionar_a_lista(NoAST *lista, NoAST *no_para_adicionar) {
    if (lista == NULL) {
        return no_para_adicionar;
    }

    NoAST *temp = lista;
    while (temp->proximo != NULL) {
        temp = temp->proximo;
    }
    
    temp->proximo = no_para_adicionar;
    return lista; 
}

/**
 * @brief Libera recursivamente toda a memória alocada para a AST.
 */
void liberar_ast(NoAST *raiz) {
    if (raiz == NULL) {
        return;
    }

    liberar_ast(raiz->filho1);
    liberar_ast(raiz->filho2);
    liberar_ast(raiz->filho3);
    liberar_ast(raiz->proximo);

    if (raiz->lexema != NULL) {
        free(raiz->lexema);
    }

    free(raiz);
}