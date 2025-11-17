#include <stdio.h>
#include <stdlib.h>
#include "../tabela_simbolos/tabela_simbolos.h"

extern FILE *yyin;      
extern int yyparse();   
extern int yylex();    
extern int yylineno;   

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

    iniciar_pilha_tabela_simbolos();

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

    eliminar_pilha_tabela_simbolos();

    return (parse_result != 0);
}
