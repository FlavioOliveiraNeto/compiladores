// Arquivo que será responsável por abrir o arquivo de entrada e iniciar a análise sintática.

#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;      // Ponteiro para o arquivo de entrada do Lexer
extern int yyparse();   // Função principal do Parser
extern int yylex();     // Função principal do Lexer (chamada por yyparse)
extern int yylineno;    // Variável global do Lexer para o número da linha

#include "modulo_arvore_sintatica_abstrata.h"        
#include "modulo_analisador_semantico.h" 
#include "modulo_gerador_codigo.h"

extern NoAST *raiz_ast;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte_goianinha>\n", argv[0]);
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return EXIT_FAILURE;
    }

    printf("Iniciando compilacao do arquivo: %s\n", argv[1]);

    int parse_result = yyparse();

    fclose(yyin);

    if (parse_result == 0) {
        printf("Analise sintatica concluida com sucesso! AST construida.\n");
        printf("Iniciando analise semantica...\n");
        int erros_semanticos = analisar_semantica(raiz_ast);
        
        if (erros_semanticos == 0) {
            printf("Analise semantica concluida com sucesso!\n");
            
            char *arquivo_saida = "saida.s";
            
            printf("Iniciando geracao de codigo para '%s'...\n", arquivo_saida);
            gerar_codigo(raiz_ast, arquivo_saida);
            printf("Geracao de codigo concluida.\n");
            
        } else {
            printf("Compilacao terminada devido a %d erros semanticos.\n", erros_semanticos);
        }

        if (raiz_ast != NULL) {
            liberar_ast(raiz_ast);
            raiz_ast = NULL;
        }

    } else {
        printf("Compilacao concluida com erros sintaticos.\n");
        if (raiz_ast != NULL) {
            liberar_ast(raiz_ast);
            raiz_ast = NULL;
        }
    }

    return (parse_result != 0);
}
