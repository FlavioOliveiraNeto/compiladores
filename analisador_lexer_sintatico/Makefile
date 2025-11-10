# Define o compilador C
CC = gcc

# Define as opções de compilação
CFLAGS = -Wall -g

# Define o nome do executável
TARGET = goianinha

# Fontes
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

# Arquivos gerados pelo Flex e Bison moderno
LEX_GEN_C = lex.yy.c
YACC_GEN_C = goianinha.tab.c
YACC_GEN_H = goianinha.tab.h

# Define os arquivos objeto
OBJS = main_compiler.o lex.yy.o goianinha.tab.o tabela_simbolos.o modulo_arvore_sintatica_abstrata.o modulo_analisador_semantico.o modulo_gerador_codigo.o

.PHONY: all clean

all: $(TARGET)

# Linkagem final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Geração do parser Bison
$(YACC_GEN_C) $(YACC_GEN_H): $(YACC_SRC)
	bison -d $(YACC_SRC)

# Geração do scanner Flex
$(LEX_GEN_C): $(LEX_SRC) $(YACC_GEN_H)
	flex $(LEX_SRC)

# Compilação do parser gerado
goianinha.tab.o: $(YACC_GEN_C) $(YACC_GEN_H)
	$(CC) $(CFLAGS) -c $(YACC_GEN_C) -o $@

# Compilação do scanner gerado
lex.yy.o: $(LEX_GEN_C) $(YACC_GEN_H)
	$(CC) $(CFLAGS) -c $(LEX_GEN_C) -o $@

# Compilação do main
main_compiler.o: $(MAIN_SRC) $(YACC_GEN_H) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(MAIN_SRC) -o $@

# Compilação da tabela de símbolos
tabela_simbolos.o: $(SYMTAB_C) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(SYMTAB_C) -o $@

# Compilação da AST
modulo_arvore_sintatica_abstrata.o: $(AST_C) $(AST_H) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(AST_C) -o $@

# Compilação do Analisador Semântico
modulo_analisador_semantico.o: $(SEMANTICA_C) $(SEMANTICA_H) $(AST_H) $(SYMTAB_H)
	$(CC) $(CFLAGS) -c $(SEMANTICA_C) -o $@

# Compilação do Gerador de Código
modulo_gerador_codigo.o: $(GERADOR_C) $(GERADOR_H) $(AST_H)
	$(CC) $(CFLAGS) -c $(GERADOR_C) -o $@

# Limpeza
clean:
	rm -f $(OBJS) $(LEX_GEN_C) $(YACC_GEN_C) $(YACC_GEN_H) $(TARGET)