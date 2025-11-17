%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tabela_simbolos/tabela_simbolos.h"
#include "modulo_arvore_sintatica_abstrata.h"

extern int yylineno;
extern char* yytext;
extern FILE *yyin;
extern int yylex();

NoAST *raiz_ast = NULL;

void yyerror(const char *s);
%}

%code requires {
    #include "../tabela_simbolos/tabela_simbolos.h"
    #include "modulo_arvore_sintatica_abstrata.h"
}

%union {
    int ival;
    char cval;
    char *sval;
    TipoDado type;
    struct NoAST *no_ast;
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
%type <no_ast> Programa
%type <no_ast> ListaDeclaracoes Declaracao RestoDeclaracao ListaDeVariaveis
%type <no_ast> ListaParametros Parametro ListaMaisParametros
%type <no_ast> ListaComandos Comando ComandoSimples Bloco
%type <no_ast> ChamadaFuncao ListaArgumentos ListaMaisArgumentos
%type <no_ast> ComandoLeia ComandoEscreva ComandoRetorne ComandoNovalinha
%type <no_ast> ComandoSe ComandoEnquanto
%type <no_ast> AssignExpr Expressao Termo Fator FatorComparacao FatorAritmetico
%type <no_ast> TermoAritmetico Atom

%%

Programa:
    ListaDeclaracoes
    KW_PROGRAMA LBRACE
        {
            printf("Iniciando analise de programa (main)\n");
        }
    ListaDeclaracoes
    ListaComandos    
    RBRACE
        {
            printf("Programa analisado com sucesso.\n");
            
            NoAST* todas_declaracoes = adicionar_a_lista($1, $5);
            
            raiz_ast = criar_no(NO_PROGRAMA, todas_declaracoes, $6, NULL);
        }
;

ListaDeclaracoes:
    /* empty */                 { $$ = NULL; }
    | ListaDeclaracoes Declaracao { $$ = adicionar_a_lista($1, $2); }
;

Declaracao:
    Tipo {  } RestoDeclaracao 
    {
        NoAST *temp = $3;
        while(temp != NULL) {
            temp->tipo_dado_computado = $1; 
            temp = temp->proximo;
        }
        $$ = $3;
    }
;

RestoDeclaracao:
    ListaDeVariaveis SEMICOLON { $$ = $1; }
    |
    ID LPAREN
        {
            
        }
      ListaParametros RPAREN Bloco
        {
            printf("Funcao '%s' analisada com sucesso.\n", $1);
          
            NoAST *no_id = criar_no_id($1);
            $$ = criar_no(NO_DECL_FUNCAO, no_id, $4, $6);

            free($1);
        }
;

ListaDeVariaveis:
    ID  {
            NoAST *no_id = criar_no_id($1);
            $$ = criar_no(NO_DECL_VARIAVEL, no_id, NULL, NULL);
            free($1);
        }
    |
    ListaDeVariaveis COMMA ID 
        {
            NoAST *no_id = criar_no_id($3);
            NoAST *nova_decl = criar_no(NO_DECL_VARIAVEL, no_id, NULL, NULL);
            $$ = adicionar_a_lista($1, nova_decl);
            free($3);
        }
;

Tipo:
    KW_INT  { $$ = TIPO_INT; }
    | KW_CAR  { $$ = TIPO_CAR; }
    | KW_VOID { $$ = TIPO_VOID; }
;

ListaParametros:
    Parametro ListaMaisParametros { $$ = adicionar_a_lista($1, $2); }
    | /* empty */                 { $$ = NULL; }
;

Parametro:
    Tipo ID
        {
            NoAST *no_id = criar_no_id($2);
            $$ = criar_no(NO_PARAMETRO, no_id, NULL, NULL);
            $$->tipo_dado_computado = $1; 
            free($2);
        }
;

ListaMaisParametros:
    COMMA Parametro ListaMaisParametros { $$ = adicionar_a_lista($2, $3); }
    | /* empty */                       { $$ = NULL; }
;

ListaComandos:
    Comando ListaComandos { $$ = adicionar_a_lista($1, $2); }
    | /* empty */         { $$ = NULL; }
;

Comando:
    ComandoSimples  { $$ = $1; }
    | Bloco         { $$ = $1; }
    | ComandoSe     { $$ = $1; }
    | ComandoEnquanto { $$ = $1; }
;

ComandoSimples:
      AssignExpr SEMICOLON      { $$ = $1; }
    | ComandoLeia SEMICOLON      { $$ = $1; }
    | ComandoEscreva SEMICOLON   { $$ = $1; }
    | ComandoRetorne SEMICOLON   { $$ = $1; }
    | ComandoNovalinha SEMICOLON { $$ = $1; }
;

Bloco:
      LBRACE
        {

        }
        ListaDeclaracoes
        ListaComandos
      RBRACE
        {

            $$ = criar_no(NO_BLOCO, $3, $4, NULL);
        }
;

ChamadaFuncao:
    ID LPAREN ListaArgumentos RPAREN
        {

            NoAST *no_id = criar_no_id($1);
            $$ = criar_no(NO_CHAMADA_FUNCAO, no_id, $3, NULL);
            free($1);
        }
;

ListaArgumentos:
    AssignExpr ListaMaisParametros { $$ = adicionar_a_lista($1, $2); }
    | /* empty */                 { $$ = NULL; }
;

ListaMaisArgumentos:
    COMMA AssignExpr ListaMaisArgumentos { $$ = adicionar_a_lista($2, $3); }
    | /* empty */                       { $$ = NULL; }
;

ComandoLeia:
    KW_LEIA ID
        {

            NoAST *no_id = criar_no_id($2);
            $$ = criar_no(NO_COMANDO_LEIA, no_id, NULL, NULL);
            free($2);
        }
;

ComandoEscreva:
    KW_ESCREVA AssignExpr        { $$ = criar_no(NO_COMANDO_ESCREVA, $2, NULL, NULL); }
  | KW_ESCREVA STRING_LITERAL { 
        NoAST *no_str = criar_no_string($2);
        $$ = criar_no(NO_COMANDO_ESCREVA, no_str, NULL, NULL);
        free($2);
    }
;

ComandoRetorne:
    KW_RETORNE AssignExpr    { $$ = criar_no(NO_COMANDO_RETORNE, $2, NULL, NULL); }
    | KW_RETORNE            { $$ = criar_no(NO_COMANDO_RETORNE, NULL, NULL, NULL); }
;

ComandoNovalinha:
    KW_NOVALINHA            { $$ = criar_no(NO_COMANDO_NOVALINHA, NULL, NULL, NULL); }
;

ComandoSe:
    KW_SE LPAREN AssignExpr RPAREN KW_ENTAO Comando %prec LOWER_THAN_ELSE
        { $$ = criar_no(NO_COMANDO_SE, $3, $6, NULL); }
    |
    KW_SE LPAREN AssignExpr RPAREN KW_ENTAO Comando KW_SENAO Comando
        { $$ = criar_no(NO_COMANDO_SE, $3, $6, $8); }
;

ComandoEnquanto:
    KW_ENQUANTO LPAREN AssignExpr RPAREN KW_EXECUTE Comando
        { $$ = criar_no(NO_COMANDO_ENQUANTO, $3, $6, NULL); }
;

AssignExpr:
    ID ASSIGN AssignExpr
        {

            NoAST *no_id = criar_no_id($1);
            $$ = criar_no(NO_COMANDO_ATRIBUICAO, no_id, $3, NULL);
            free($1);
        }
    | Expressao { $$ = $1; }
;

Expressao:
    Expressao OR Termo  { $$ = criar_no(NO_OP_OU, $1, $3, NULL); }
    | Termo             { $$ = $1; }
;

Termo:
    Termo AND Fator     { $$ = criar_no(NO_OP_E, $1, $3, NULL); }
    | Fator             { $$ = $1; }
;

Fator:
    FatorComparacao     { $$ = $1; }
    | NOT Fator         { $$ = criar_no(NO_OP_NEGACAO, $2, NULL, NULL); }
;

FatorComparacao:
      FatorAritmetico EQ FatorAritmetico  { $$ = criar_no(NO_OP_IGUAL, $1, $3, NULL); }
    | FatorAritmetico NE FatorAritmetico  { $$ = criar_no(NO_OP_DIFERENTE, $1, $3, NULL); }
    | FatorAritmetico LT FatorAritmetico  { $$ = criar_no(NO_OP_MENOR, $1, $3, NULL); }
    | FatorAritmetico GT FatorAritmetico  { $$ = criar_no(NO_OP_MAIOR, $1, $3, NULL); }
    | FatorAritmetico LE FatorAritmetico  { $$ = criar_no(NO_OP_MENOR_IGUAL, $1, $3, NULL); }
    | FatorAritmetico GE FatorAritmetico  { $$ = criar_no(NO_OP_MAIOR_IGUAL, $1, $3, NULL); }
    | FatorAritmetico                     { $$ = $1; }
;

FatorAritmetico:
    FatorAritmetico SUM TermoAritmetico { $$ = criar_no(NO_OP_SOMA, $1, $3, NULL); }
    | FatorAritmetico SUB TermoAritmetico { $$ = criar_no(NO_OP_SUB, $1, $3, NULL); }
    | TermoAritmetico                     { $$ = $1; }
;

TermoAritmetico:
    TermoAritmetico MUL Atom    { $$ = criar_no(NO_OP_MULT, $1, $3, NULL); }
    | TermoAritmetico DIV Atom  { $$ = criar_no(NO_OP_DIV, $1, $3, NULL); }
    | Atom                      { $$ = $1; }
;

Atom:
      ID
        {
            
            $$ = criar_no_id($1);
            free($1);
        }
    | INT_LITERAL
        {
            $$ = criar_no_int($1);
        }
    | CHAR_LITERAL
        {
            $$ = criar_no_car($1);
        }
    | STRING_LITERAL
        {
            $$ = criar_no_string($1);
            free($1);
        }
    | LPAREN AssignExpr RPAREN
        {
            $$ = $2;
        }
    | ChamadaFuncao
        {
            $$ = $1;
        }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "ERRO: %s na linha %d\n", s, yylineno);
}