#ifndef GERADOR_CODIGO_H
#define GERADOR_CODIGO_H

#include "modulo_arvore_sintatica_abstrata.h"

/**
 * @brief Função principal do Gerador de Código.
 * * Percorre a Árvore Sintática Abstrata (AST), que já foi validada
 * semanticamente, e gera o código assembly MIPS correspondente.
 * * @param raiz O nó raiz da AST.
 * @param nome_arquivo_saida O nome do arquivo (ex: "saida.s") onde
 * o código assembly MIPS será escrito.
 */
void gerar_codigo(NoAST *raiz, const char *nome_arquivo_saida);


#endif