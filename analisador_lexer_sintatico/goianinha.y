%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tabela_simbolos/tabela_simbolos.h"

extern int yylineno;
extern char* yytext;
extern FILE *yyin;
extern int yylex();
extern TabelaSimbolos *pilha_tabelas;

TipoDado tipo_atual_declaracao;
int contador_posicao_escopo = 0;
EntradaTabela *funcao_atual_declaracao = NULL;

void yyerror(const char *s);
%}

%code requires {
    #include "../tabela_simbolos/tabela_simbolos.h"
}

%union {
    int ival;
    char cval;
    char *sval;
    TipoDado type;
}

%token <sval> ID STRING_LITERAL
%token <ival> INT_LITERAL
%token <cval> CHAR_LITERAL

%token KW_PROGRAMA KW_CAR KW_INT KW_RETORNE KW_LEIA KW_ESCREVA KW_NOVALINHA KW_SE KW_ENTAO KW_SENAO KW_ENQUANTO KW_EXECUTE KW_VOID
%token SUM SUB MUL DIV ASSIGN EQ NE LT GT LE GE AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA

%nonassoc LOWER_THAN_ELSE
%nonassoc KW_SENAO

%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left SUM SUB
%left MUL DIV
%right NOT
%right ASSIGN

%type <type> Tipo
%type <sval> Parametro

%%

Programa:
    KW_PROGRAMA ID LBRACE
        {
            printf("Iniciando analise de programa '%s'\n", $2);
            iniciar_pilha_tabela_simbolos();
            criar_novo_escopo_e_empilhar();
            contador_posicao_escopo = 0;
        }
        ListaDeclaracoes
        ListaComandos
    RBRACE
        {
            printf("Programa '%s' analisado com sucesso.\n", $2);
            remover_escopo_atual();
            eliminar_pilha_tabela_simbolos();
        }
;

ListaDeclaracoes:
    /* empty */
    | ListaDeclaracoes Declaracao
;

Declaracao:
    Tipo { tipo_atual_declaracao = $1; } RestoDeclaracao
;

RestoDeclaracao:
    ListaDeVariaveis SEMICOLON
    | ID LPAREN
        {
            funcao_atual_declaracao = inserir_funcao_na_tabela_atual($1, tipo_atual_declaracao, 0);
            criar_novo_escopo_e_empilhar();
            contador_posicao_escopo = 0;
        }
      ListaParametros RPAREN Bloco
        {
            printf("Funcao '%s' analisada com sucesso.\n", $1);
            remover_escopo_atual();
            funcao_atual_declaracao = NULL;
        }
;

ListaDeVariaveis:
    ID { inserir_variavel_na_tabela_atual($1, tipo_atual_declaracao, contador_posicao_escopo++); }
    | ListaDeVariaveis COMMA ID { inserir_variavel_na_tabela_atual($3, tipo_atual_declaracao, contador_posicao_escopo++); }
;

Tipo:
    KW_INT  { $$ = TIPO_INT; }
    | KW_CAR  { $$ = TIPO_CAR; }
    | KW_VOID { $$ = TIPO_VOID; }
;

ListaParametros:
    Parametro ListaMaisParametros
    | /* empty */
;

Parametro:
    Tipo ID
        {
            inserir_parametro_na_tabela_atual($2, $1, contador_posicao_escopo++);
            if (funcao_atual_declaracao) {
                adicionar_parametro_a_funcao(funcao_atual_declaracao, $2, $1);
                funcao_atual_declaracao->num_argumentos = contador_posicao_escopo;
            }
        }
;

ListaMaisParametros:
    COMMA Parametro ListaMaisParametros
    | /* empty */
;

ListaComandos:
    Comando ListaComandos
    | /* empty */
;

Comando:
    ComandoSimples
    | Bloco
    | ComandoSe
    | ComandoEnquanto
;

ComandoSimples:
      Atribuicao SEMICOLON
    | ChamadaFuncao SEMICOLON
    | ComandoLeia SEMICOLON
    | ComandoEscreva SEMICOLON
    | ComandoRetorne SEMICOLON
    | ComandoNovalinha SEMICOLON
;

Bloco:
      LBRACE
        {
            if (funcao_atual_declaracao == NULL) {
                criar_novo_escopo_e_empilhar();
                contador_posicao_escopo = 0;
            }
        }
        ListaDeclaracoes
        ListaComandos
      RBRACE
        {
            if (funcao_atual_declaracao == NULL) {
                 remover_escopo_atual();
            }
        }
;

Atribuicao:
    ID ASSIGN Expressao
        {
            if (!pesquisar_nome_na_pilha($1)) {
                fprintf(stderr, "ERRO: Variavel '%s' nao declarada na linha %d\n", $1, yylineno);
            }
        }
;

ChamadaFuncao:
    ID LPAREN ListaArgumentos RPAREN
        {
            EntradaTabela *func_entry = pesquisar_nome_na_pilha($1);
            if (!func_entry || func_entry->tipo != TIPO_FUNCAO) {
                fprintf(stderr, "ERRO: Funcao '%s' nao declarada ou tipo incorreto na linha %d\n", $1, yylineno);
            }
        }
;

ListaArgumentos:
    Expressao ListaMaisArgumentos
    | /* empty */
;

ListaMaisArgumentos:
    COMMA Expressao ListaMaisArgumentos
    | /* empty */
;

ComandoLeia:
    KW_LEIA ID
        {
            if (!pesquisar_nome_na_pilha($2)) {
                fprintf(stderr, "ERRO: Variavel '%s' nao declarada na linha %d\n", $2, yylineno);
            }
        }
;

ComandoEscreva:
    KW_ESCREVA Expressao
  | KW_ESCREVA STRING_LITERAL
;

ComandoRetorne:
    KW_RETORNE Expressao
    | KW_RETORNE
;

ComandoNovalinha:
    KW_NOVALINHA
;

ComandoSe:
    KW_SE LPAREN Expressao RPAREN KW_ENTAO Comando %prec LOWER_THAN_ELSE
    | KW_SE LPAREN Expressao RPAREN KW_ENTAO Comando KW_SENAO Comando
;

ComandoEnquanto:
    KW_ENQUANTO LPAREN Expressao RPAREN KW_EXECUTE Comando
;

Expressao:
    Expressao OR Termo
    | Termo
;
Termo:
    Termo AND Fator
    | Fator
;
Fator:
      FatorComparacao
    | NOT Fator
;
FatorComparacao:
      FatorAritmetico EQ FatorAritmetico
    | FatorAritmetico NE FatorAritmetico
    | FatorAritmetico LT FatorAritmetico
    | FatorAritmetico GT FatorAritmetico
    | FatorAritmetico LE FatorAritmetico
    | FatorAritmetico GE FatorAritmetico
    | FatorAritmetico
;
FatorAritmetico:
    FatorAritmetico SUM TermoAritmetico
    | FatorAritmetico SUB TermoAritmetico
    | TermoAritmetico
;
TermoAritmetico:
    TermoAritmetico MUL Atom
    | TermoAritmetico DIV Atom
    | Atom
;
Atom:
      ID
        {
            if (!pesquisar_nome_na_pilha($1)) {
                fprintf(stderr, "ERRO: Variavel '%s' nao declarada na linha %d\n", $1, yylineno);
            }
        }
    | INT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | LPAREN Expressao RPAREN
    | ChamadaFuncao
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO: %s na linha %d\n", s, yylineno);
}