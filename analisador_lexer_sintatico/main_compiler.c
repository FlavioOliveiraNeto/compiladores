// Arquivo que será responsável por abrir o arquivo de entrada e iniciar a análise sintática.

#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;      // Ponteiro para o arquivo de entrada do Lexer
extern int yyparse();   // Função principal do Parser
extern int yylex();     // Função principal do Lexer (chamada por yyparse)
extern int yylineno;    // Variável global do Lexer para o número da linha

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
        printf("Compilacao concluida com sucesso! Nao foram encontrados erros sintaticos.\n");
    } else {
        printf("Compilacao concluida com erros sintaticos.\n");
    }

    return parse_result;
}
