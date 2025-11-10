#ifndef SEMANTICA_H
#define SEMANTICA_H

#include "modulo_arvore_sintatica_abstrata.h"
#include "../tabela_simbolos/tabela_simbolos.h"

/**
 * @brief Função principal do analisador semântico.
 * * Percorre a Árvore Sintática Abstrata (AST) para realizar:
 * 1. Análise de Escopo: Verifica se variáveis e funções são declaradas antes do uso.
 * 2. Checagem de Tipos: Verifica a compatibilidade de tipos em atribuições,
 * operações aritméticas, lógicas e chamadas de função[cite: 124, 131, 132, 133].
 * * Esta função também preenche os campos 'tipo_dado_computado' e 'entrada_tabela'
 * nos nós da AST para uso posterior pelo gerador de código.
 * * @param raiz O nó raiz da AST (gerado pelo yyparse).
 * @return O número total de erros semânticos encontrados.
 */
int analisar_semantica(NoAST *raiz);


#endif // SEMANTICA_H