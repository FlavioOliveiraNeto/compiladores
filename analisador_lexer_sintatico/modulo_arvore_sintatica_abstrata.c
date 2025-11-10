#include "modulo_arvore_sintatica_abstrata.h"

/**
 * @brief Aloca e inicializa um nó genérico da AST.
 * Esta é a função base para a criação de nós.
 */
static NoAST* alocar_no() {
    NoAST *no = (NoAST*) malloc(sizeof(NoAST));
    if (no == NULL) {
        fprintf(stderr, "Erro critico: Falha ao alocar memoria para NoAST.\n");
        exit(EXIT_FAILURE);
    }
    
    // Inicializa todos os campos para um estado padrão
    no->tipo_no = NO_PROGRAMA; // Tipo padrão (será sobrescrito)
    no->linha = yylineno; // Pega a linha atual do lexer
    no->filho1 = NULL;
    no->filho2 = NULL;
    no->filho3 = NULL;
    no->proximo = NULL;
    no->lexema = NULL;
    no->ival = 0;
    no->cval = '\0';
    no->tipo_dado_computado = TIPO_VOID; // Tipo semântico padrão
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
    no->lexema = strdup(lexema); // Copia o lexema para a árvore
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
    no->lexema = strdup(str); // Copia a string para a árvore
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
        return no_para_adicionar; // O nó se torna a cabeça da lista
    }

    // Percorre a lista até encontrar o último nó
    NoAST *temp = lista;
    while (temp->proximo != NULL) {
        temp = temp->proximo;
    }
    
    // Adiciona o novo nó no final
    temp->proximo = no_para_adicionar;
    return lista; // Retorna a cabeça original da lista
}

/**
 * @brief Libera recursivamente toda a memória alocada para a AST.
 */
void liberar_ast(NoAST *raiz) {
    if (raiz == NULL) {
        return;
    }

    // Libera recursivamente os filhos
    liberar_ast(raiz->filho1);
    liberar_ast(raiz->filho2);
    liberar_ast(raiz->filho3);
    
    // Libera o próximo nó na lista (se houver)
    liberar_ast(raiz->proximo);

    // Libera dados alocados dinamicamente (lexemas)
    if (raiz->lexema != NULL) {
        free(raiz->lexema);
    }

    // Libera o próprio nó
    free(raiz);
}