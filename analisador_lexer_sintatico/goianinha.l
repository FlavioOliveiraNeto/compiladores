%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "goianinha.tab.h"
#include "../tabela_simbolos/tabela_simbolos.h"

int comment_level = 0;
extern void yyerror(const char *s);
%}

%option noyywrap
%option yylineno

%x COMMENT

%%

"programa"      { return KW_PROGRAMA; }
"car"           { return KW_CAR; }
"int"           { return KW_INT; }
"retorne"       { return KW_RETORNE; }
"leia"          { return KW_LEIA; }
"escreva"       { return KW_ESCREVA; }
"novalinha"     { return KW_NOVALINHA; }
"se"            { return KW_SE; }
"entao"         { return KW_ENTAO; }
"senao"         { return KW_SENAO; }
"enquanto"      { return KW_ENQUANTO; }
"execute"       { return KW_EXECUTE; }
"void"          { return KW_VOID; }

"+" { return SUM; }
"-" { return SUB; }
"*" { return MUL; }
"/" { return DIV; }
"=" { return ASSIGN; }
"==" { return EQ; }
"!=" { return NE; }
"<" { return LT; }
">" { return GT; }
"<=" { return LE; }
">=" { return GE; }
"&&" { return AND; }
"||" { return OR; }
"!" { return NOT; }

"(" { return LPAREN; }
")" { return RPAREN; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"[" { return LBRACKET; }
"]" { return RBRACKET; }
";" { return SEMICOLON; }
"," { return COMMA; }

[0-9]+          { yylval.ival = atoi(yytext); return INT_LITERAL; }
\'[^\'\n]\'     { yylval.cval = yytext[1]; return CHAR_LITERAL; }
\"(\\.|[^"\n])*\" {
    yylval.sval = strdup(yytext + 1);
    yylval.sval[strlen(yylval.sval)-1] = '\0';
    return STRING_LITERAL;
}

[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return ID; }

"/*"            { BEGIN(COMMENT); comment_level++; }
"//"            { while (input() != '\n' && input() != EOF); }
<COMMENT>{
    "*/"        {
                    comment_level--;
                    if (comment_level == 0) {
                        BEGIN(INITIAL);
                    }
                }
    "/*"        { comment_level++; }
    \n          { /* conta linhas em comentarios */ }
    .           { /* ignora */ }
    <<EOF>>     {
        if (comment_level > 0) {
            fprintf(stderr, "ERRO: COMENTÁRIO NAO TERMINA na linha %d\n", yylineno);
        }
        yyterminate();
    }
}
\"[^\"]*\n      { fprintf(stderr, "ERRO: CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA na linha %d\n", yylineno); }

[ \t\r]+        { /* ignora */ }
\n              { /* Flex já incrementa yylineno */ }
.               {
    fprintf(stderr, "ERRO: CARACTERE INVÁLIDO na linha %d\n", yylineno);
}
%%