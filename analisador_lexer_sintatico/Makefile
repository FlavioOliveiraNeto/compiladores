CC = gcc
CFLAGS = -Wall -g
TARGET = goianinha
LEX_SRC = goianinha.l
YACC_SRC = goianinha.y
MAIN_SRC = main_compiler.c
SYMTAB_C = ../tabela_simbolos/tabela_simbolos.c
SYMTAB_H = ../tabela_simbolos/tabela_simbolos.h
AST_C = modulo_arvore_sintatica_abstrata.c
AST_H = modulo_arvore_sintatica_abstrata.h
SEMANTICA_C = modulo_analisador_semantico.c
SEMANTICA_H = modulo_analisador_semantico.h
GERADOR_C = modulo_gerador_codigo.c
GERADOR_H = modulo_gerador_codigo.h
LEX_GEN_C = lex.yy.c
YACC_GEN_C = goianinha.tab.c
YACC_GEN_H = goianinha.tab.h
OBJS = main_compiler.o lex.yy.o goianinha.tab.o tabela_simbolos.o modulo_arvore_sintatica_abstrata.o modulo_analisador_semantico.o modulo_gerador_codigo.o
.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
$(YACC_GEN_C) $(YACC_GEN_H): $(YACC_SRC)
	bison -d $(YACC_SRC)
$(LEX_GEN_C): $(LEX_SRC) $(YACC_GEN_H)
	flex $(LEX_SRC)
goianinha.tab.o: $(YACC_GEN_C) $(YACC_GEN_H)
	$(CC) $(CFLAGS) -c $(YACC_GEN_C) -o $@
lex.yy.o: $(LEX_GEN_C) $(YACC_GEN_H)
	$(CC) $(CFLAGS) -c $(LEX_GEN_C) -o $@
main_compiler.o: $(MAIN_SRC) $(YACC_GEN_H) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(MAIN_SRC) -o $@
tabela_simbolos.o: $(SYMTAB_C) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(SYMTAB_C) -o $@
modulo_arvore_sintatica_abstrata.o: $(AST_C) $(AST_H) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(AST_C) -o $@
modulo_analisador_semantico.o: $(SEMANTICA_C) $(SEMANTICA_H) $(AST_H) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(SEMANTICA_C) -o $@
modulo_gerador_codigo.o: $(GERADOR_C) $(GERADOR_H) $(AST_H)
	$(CC) $(CFLAGS) -c $(GERADOR_C) -o $@
clean:
	rm -f $(OBJS) $(LEX_GEN_C) $(YACC_GEN_C) $(YACC_GEN_H) $(TARGET)