#include "modulo_gerador_codigo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *arquivo_saida;

static int contador_label_se = 0;
static int contador_label_enquanto = 0;
static int contador_label_string = 0;

static int offset_pilha_local = 0;

static NoAST *funcao_sendo_gerada = NULL;

static void percorrer_geracao(NoAST *no);

static void gerar_programa(NoAST *no);
static void gerar_bloco(NoAST *no);
static void gerar_lista_declaracoes(NoAST *no);
static void gerar_lista_comandos(NoAST *no);
static void gerar_declaracao_funcao(NoAST *no);
static void gerar_declaracao_variavel(NoAST *no);
static void gerar_comando_atribuicao(NoAST *no);
static void gerar_comando_se(NoAST *no);
static void gerar_comando_enquanto(NoAST *no);
static void gerar_comando_leia(NoAST *no);
static void gerar_comando_escreva(NoAST *no);
static void gerar_comando_retorne(NoAST *no);
static void gerar_chamada_funcao(NoAST *no);
static void gerar_novalinha();

static void gerar_expressao(NoAST *no);
static void gerar_op_binaria(NoAST *no, const char *mnemonico_mips);
static void gerar_op_logica(NoAST *no, const char *mnemonico_mips);
static void gerar_id(NoAST *no);

static void gerar_endereco_variavel(NoAST *no_id);

static void emitir(const char *codigo_mips);
static void emitir_comentario(const char *comentario);

/**
 * @brief Função principal (pública) que inicia a geração de código.
 */
void gerar_codigo(NoAST *raiz, const char *nome_arquivo_saida) {
    arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (arquivo_saida == NULL) {
        perror("Erro ao abrir arquivo de saida MIPS");
        exit(EXIT_FAILURE);
    }

    contador_label_se = 0;
    contador_label_enquanto = 0;
    contador_label_string = 0;
    funcao_sendo_gerada = NULL;
    offset_pilha_local = 0;

    percorrer_geracao(raiz);

    fclose(arquivo_saida);
}

/**
 * @brief Função "dispatcher" que chama o gerador correto para cada nó.
 */
static void percorrer_geracao(NoAST *no) {
    if (no == NULL) return;

    switch (no->tipo_no) {
        case NO_PROGRAMA:           gerar_programa(no); break;
        case NO_BLOCO:              gerar_bloco(no); break;
        case NO_LISTA_DECLARACOES:  gerar_lista_declaracoes(no); break;
        case NO_LISTA_COMANDOS:     gerar_lista_comandos(no); break;
        case NO_DECL_FUNCAO:        gerar_declaracao_funcao(no); break;
        case NO_DECL_VARIAVEL:      gerar_declaracao_variavel(no); break;
        case NO_COMANDO_ATRIBUICAO: gerar_comando_atribuicao(no); break;
        case NO_COMANDO_SE:         gerar_comando_se(no); break;
        case NO_COMANDO_ENQUANTO:   gerar_comando_enquanto(no); break;
        case NO_COMANDO_LEIA:       gerar_comando_leia(no); break;
        case NO_COMANDO_ESCREVA:    gerar_comando_escreva(no); break;
        case NO_COMANDO_RETORNE:    gerar_comando_retorne(no); break;
        case NO_COMANDO_NOVALINHA:  gerar_novalinha(); break;
        
        case NO_CHAMADA_FUNCAO:
        case NO_OP_SOMA:
        case NO_OP_SUB:
        case NO_OP_MULT:
        case NO_OP_DIV:
        case NO_OP_IGUAL:
        case NO_OP_DIFERENTE:
        case NO_OP_MENOR:
        case NO_OP_MAIOR:
        case NO_OP_MENOR_IGUAL:
        case NO_OP_MAIOR_IGUAL:
        case NO_OP_E:
        case NO_OP_OU:
        case NO_OP_NEGACAO:
        case NO_ID:
        case NO_LITERAL_INT:
        case NO_LITERAL_CAR:
            if (no->tipo_no == NO_CHAMADA_FUNCAO) {
                gerar_chamada_funcao(no);
            } else {
                 emitir_comentario("AVISO: Expressao encontrada fora de contexto de comando.");
            }
            break;
            
        case NO_LITERAL_STRING:
        case NO_PARAMETRO:
        case NO_LISTA_PARAMETROS:
        case NO_LISTA_ARGUMENTOS:
            break;
        
        default:
            fprintf(stderr, "Erro de geracao: No AST desconhecido (%d)\n", no->tipo_no);
            break;
    }
}


static void emitir(const char *codigo_mips) {
    fprintf(arquivo_saida, "\t%s\n", codigo_mips);
}

static void emitir_com_valor(const char *codigo_mips, int valor) {
    fprintf(arquivo_saida, "\t%s %d\n", codigo_mips, valor);
}

static void emitir_com_label(const char *codigo_mips, const char *label) {
    fprintf(arquivo_saida, "\t%s %s\n", codigo_mips, label);
}

static void emitir_label(const char *label) {
    fprintf(arquivo_saida, "%s:\n", label);
}

static void emitir_comentario(const char *comentario) {
    fprintf(arquivo_saida, "\n\t# %s\n", comentario);
}

/**
 * @brief Configura o MIPS com as seções .data e .text.
 */
static void gerar_programa(NoAST *no) {
    emitir(".data");
    emitir_label("newline");
    emitir(".asciiz \"\\n\"");
    emitir("\n.text");
    emitir(".globl main");
    
    emitir_comentario("--- Declaracoes de Funcoes ---");
    percorrer_geracao(no->filho1);

    emitir_comentario("--- Programa Principal (main) ---");
    emitir_label("main");
    offset_pilha_local = 0;
    NoAST* decls_main = no->filho1;
    NoAST* cmds_main = no->filho2;
    
    int espaco_locais_main = 0;
    NoAST* temp_decl = decls_main;
    while(temp_decl != NULL) {
        if(temp_decl->tipo_no == NO_DECL_VARIAVEL) {
            espaco_locais_main += 4; 
            temp_decl->filho1->entrada_tabela->posicao = -espaco_locais_main;
        }
        temp_decl = temp_decl->proximo;
    }

    if (espaco_locais_main > 0) {
        emitir_com_valor("addiu $sp, $sp,", -espaco_locais_main);
    }
    emitir("move $fp, $sp"); 

    gerar_lista_comandos(cmds_main); 

    emitir_comentario("--- Fim do Programa (exit) ---");
    if (espaco_locais_main > 0) {
        emitir_com_valor("addiu $sp, $sp,", espaco_locais_main); 
    }
    emitir("li $v0, 10");
    emitir("syscall");
}

/**
 * @brief Gera código para um bloco (lista de comandos).
 * As declarações já foram tratadas pelo 'pai' (função ou programa).
 */
static void gerar_bloco(NoAST *no) {
    gerar_lista_comandos(no->filho2);
}

/**
 * @brief Percorre uma lista de declarações.
 */
static void gerar_lista_declaracoes(NoAST *no) {
    NoAST *temp = no;
    while (temp != NULL) {
        percorrer_geracao(temp);
        temp = temp->proximo;
    }
}

/**
 * @brief Percorre uma lista de comandos.
 */
static void gerar_lista_comandos(NoAST *no) {
    NoAST *temp = no;
    while (temp != NULL) {
        percorrer_geracao(temp);
        temp = temp->proximo;
    }
}

/**
 * @brief Gera o prólogo, corpo e epílogo de uma função.
 */
static void gerar_declaracao_funcao(NoAST *no) {
    char label_funcao[100];
    sprintf(label_funcao, "func_%s", no->filho1->lexema);
    emitir_label(label_funcao);

    funcao_sendo_gerada = no;
    offset_pilha_local = 0; 

    emitir_comentario("Prologo");
    emitir("addiu $sp, $sp, -8");
    emitir("sw $ra, 4($sp)");
    emitir("sw $fp, 0($sp)");
    emitir("move $fp, $sp");

    int espaco_locais = 0;
    NoAST *lista_decls_locais = no->filho3->filho1;
    NoAST *temp_decl = lista_decls_locais;
    while (temp_decl != NULL) {
        if (temp_decl->tipo_no == NO_DECL_VARIAVEL) {
            espaco_locais += 4; 
            temp_decl->filho1->entrada_tabela->posicao = -espaco_locais;
        }
        temp_decl = temp_decl->proximo;
    }
    
    if (espaco_locais > 0) {
        emitir_com_valor("addiu $sp, $sp,", -espaco_locais);
    }
    offset_pilha_local = espaco_locais; 

    emitir_comentario("Corpo da Funcao");
    percorrer_geracao(no->filho3); 

    emitir_comentario("Epilogo");
    char label_retorno[100];
    sprintf(label_retorno, "ret_%s", no->filho1->lexema);
    emitir_label(label_retorno);

    if (offset_pilha_local > 0) {
        emitir_com_valor("addiu $sp, $sp,", offset_pilha_local);
    }
    emitir("lw $ra, 4($sp)");
    emitir("lw $fp, 0($sp)");
    emitir("addiu $sp, $sp, 8");
    
    emitir("jr $ra");
    
    funcao_sendo_gerada = NULL;
}

/**
 * @brief Processa uma declaração de variável (calcula offset).
 * (Já feito em gerar_programa e gerar_declaracao_funcao)
 */
static void gerar_declaracao_variavel(NoAST *no) {
}

/**
 * @brief Gera código para carregar o endereço de uma variável em $t0.
 */
static void gerar_endereco_variavel(NoAST *no_id) {
    if (no_id->tipo_no != NO_ID) return;
    
    EntradaTabela *entrada = no_id->entrada_tabela;
    if (entrada == NULL) {
        fprintf(stderr, "Erro de geracao: ID '%s' sem entrada na tabela.\n", no_id->lexema);
        return;
    }
    
    
    if (entrada->posicao >= 0) { 
        
        int num_args = funcao_sendo_gerada->filho1->entrada_tabela->num_argumentos;
        int offset = 8 + 4 * (num_args - 1 - entrada->posicao);
        emitir_com_valor("addiu $t0, $fp,", offset); 
        
    } else { 
        emitir_com_valor("addiu $t0, $fp,", entrada->posicao); 
    }
}

/**
 * @brief Gera código para uma atribuição (ID = Expr).
 */
static void gerar_comando_atribuicao(NoAST *no) {
    emitir_comentario("Atribuicao");
    gerar_endereco_variavel(no->filho1);
    
    emitir("addiu $sp, $sp, -4");
    emitir("sw $t0, 0($sp)");
    
    gerar_expressao(no->filho2);
    
    emitir("lw $t0, 0($sp)");
    emitir("addiu $sp, $sp, 4");
    
    emitir("sw $v0, 0($t0)"); 
}

/**
 * @brief Gera código para um comando 'se' (if-then-else).
 */
static void gerar_comando_se(NoAST *no) {
    int label_id = contador_label_se++;
    char label_else[20];
    char label_fim[20];
    sprintf(label_else, "se_else_%d", label_id);
    sprintf(label_fim, "se_fim_%d", label_id);
    
    emitir_comentario("Comando SE");
    
    gerar_expressao(no->filho1);
    
    if (no->filho3 != NULL) { 
        emitir_com_label("beqz $v0,", label_else); 
    } else { 
        emitir_com_label("beqz $v0,", label_fim); 
    }
    

    emitir_comentario("SE - Bloco THEN");
    percorrer_geracao(no->filho2);
    
    if (no->filho3 != NULL) { 
        emitir_com_label("j", label_fim); 
        emitir_label(label_else);
        emitir_comentario("SE - Bloco ELSE");
        percorrer_geracao(no->filho3); 
    }

    emitir_label(label_fim);
}

/**
 * @brief Gera código para um comando 'enquanto'.
 */
static void gerar_comando_enquanto(NoAST *no) {
    int label_id = contador_label_enquanto++;
    char label_inicio[20];
    char label_fim[20];
    sprintf(label_inicio, "enquanto_inicio_%d", label_id);
    sprintf(label_fim, "enquanto_fim_%d", label_id);
    
    emitir_comentario("Comando ENQUANTO");
    emitir_label(label_inicio); 
    
    gerar_expressao(no->filho1); 
    
    emitir_com_label("beqz $v0,", label_fim); 
    
    emitir_comentario("ENQUANTO - Corpo");
    percorrer_geracao(no->filho2);
    
    emitir_com_label("j", label_inicio);
    
    emitir_label(label_fim);
}

/**
 * @brief Gera código para o comando 'leia'.
 */
static void gerar_comando_leia(NoAST *no) {
    emitir_comentario("Comando LEIA");
    emitir("li $v0, 5");
    emitir("syscall"); 
    
    gerar_endereco_variavel(no->filho1); 
    emitir("sw $v0, 0($t0)"); 
}

/**
 * @brief Gera código para o comando 'escreva'.
 */
static void gerar_comando_escreva(NoAST *no) {
    
    if (no->filho1->tipo_no == NO_LITERAL_STRING) {
        emitir_comentario("Comando ESCREVA (String)");
        char label_str[20];
        sprintf(label_str, "str_%d", contador_label_string++);
        emitir(".data");
        emitir_label(label_str);
        fprintf(arquivo_saida, "\t.asciiz \"%s\"\n", no->filho1->lexema);
        emitir(".text");
        
        emitir_com_label("la $a0,", label_str);
        emitir("li $v0, 4");
        emitir("syscall");
        
    } else {
        emitir_comentario("Comando ESCREVA (Expressao)");
        gerar_expressao(no->filho1);
        
        emitir("move $a0, $v0");
        
        if (no->filho1->tipo_dado_computado == TIPO_CAR) {
            emitir("li $v0, 11");
        } else {
            emitir("li $v0, 1");
        }
        emitir("syscall");
    }
}

/**
 * @brief Gera código para o comando 'retorne'.
 */
static void gerar_comando_retorne(NoAST *no) {
    emitir_comentario("Comando RETORNE");
    if (no->filho1 != NULL) { 
        gerar_expressao(no->filho1);
    }
    
    char label_retorno[100];
    sprintf(label_retorno, "ret_%s", funcao_sendo_gerada->filho1->lexema);
    emitir_com_label("j", label_retorno);
}

/**
 * @brief Gera código para 'novalinha'.
 */
static void gerar_novalinha() {
    emitir_comentario("Comando NOVALINHA");
    emitir("la $a0, newline");
    emitir("li $v0, 4");
    emitir("syscall");
}

/**
 * @brief Gera código para uma chamada de função.
 */
static void gerar_chamada_funcao(NoAST *no) {
    emitir_comentario("Chamada de Funcao");
    int num_args = 0;
    NoAST *arg = no->filho2;
    while (arg != NULL) {
        gerar_expressao(arg); 
        emitir("addiu $sp, $sp, -4"); 
        emitir("sw $v0, 0($sp)");
        num_args++;
        arg = arg->proximo;
    }
    
    char label_funcao[100];
    sprintf(label_funcao, "func_%s", no->filho1->lexema);
    emitir_com_label("jal", label_funcao);
    
    if (num_args > 0) {
        emitir_com_valor("addiu $sp, $sp,", num_args * 4);
    }
    
}


/**
 * @brief Função "dispatcher" para gerar código para qualquer expressão.
 * O resultado da expressão é sempre deixado no registrador $v0.
 */
static void gerar_expressao(NoAST *no) {
    if (no == NULL) return;

    switch (no->tipo_no) {
        case NO_LITERAL_INT:
            emitir_com_valor("li $v0,", no->ival);
            break;
        case NO_LITERAL_CAR:
            emitir_com_valor("li $v0,", no->cval);
            break;
        case NO_ID:
            gerar_id(no);
            break;
        case NO_CHAMADA_FUNCAO:
            gerar_chamada_funcao(no);
            break;
        case NO_COMANDO_ATRIBUICAO:
            gerar_comando_atribuicao(no);
            break;
            
        // Operações Binárias
        case NO_OP_SOMA:    gerar_op_binaria(no, "add"); break;
        case NO_OP_SUB:     gerar_op_binaria(no, "sub"); break;
        case NO_OP_MULT:    gerar_op_binaria(no, "mult"); break;
        case NO_OP_DIV:     gerar_op_binaria(no, "div"); break; 
        
        case NO_OP_IGUAL:   gerar_op_logica(no, "seq"); break; 
        case NO_OP_DIFERENTE: gerar_op_logica(no, "sne"); break; 
        case NO_OP_MENOR:   gerar_op_logica(no, "slt"); break; 
        case NO_OP_MAIOR:   gerar_op_logica(no, "sgt"); break;
        case NO_OP_MENOR_IGUAL: gerar_op_logica(no, "sle"); break; 
        case NO_OP_MAIOR_IGUAL: gerar_op_logica(no, "sge"); break; 
        
        case NO_OP_E:       gerar_op_binaria(no, "and"); break; 
        case NO_OP_OU:      gerar_op_binaria(no, "or"); break;  
        
        case NO_OP_NEGACAO: 
            emitir_comentario("Expressao NOT");
            gerar_expressao(no->filho1); 
            emitir("seq $v0, $v0, $zero");
            break;

        default:
            fprintf(stderr, "Erro de geracao: No de expressao desconhecido (%d)\n", no->tipo_no);
            break;
    }
}

/**
 * @brief Gera código para carregar o valor de um ID em $v0.
 */
static void gerar_id(NoAST *no) {
    gerar_endereco_variavel(no); 
    emitir("lw $v0, 0($t0)"); 
}

/**
 * @brief Gera código para uma operação binária (Soma, Sub, etc.).
 * Padrão: (1) Gera LHS, (2) Empilha, (3) Gera RHS, (4) Desempilha, (5) Opera.
 * Resultado final em $v0.
 */
static void gerar_op_binaria(NoAST *no, const char *mnemonico_mips) {
    emitir_comentario("Expressao Binaria");
    gerar_expressao(no->filho1); 
    
    emitir("addiu $sp, $sp, -4");
    emitir("sw $v0, 0($sp)");
    
    gerar_expressao(no->filho2); 
    
    emitir("lw $t1, 0($sp)");
    emitir("addiu $sp, $sp, 4");
    
    if (strcmp(mnemonico_mips, "mult") == 0) {
        emitir("mult $t1, $v0");
        emitir("mflo $v0");
    } else if (strcmp(mnemonico_mips, "div") == 0) {
        emitir("div $t1, $v0"); 
        emitir("mflo $v0");
    } else {
        char instrucao[50];
        sprintf(instrucao, "%s $v0, $t1, $v0", mnemonico_mips);
        emitir(instrucao);
    }
}

/**
 * @brief Gera código para uma operação lógica/relacional (SEQ, SNE, SLT, ...).
 * Similar à binária, mas o mnemônico é diferente.
 */
static void gerar_op_logica(NoAST *no, const char *mnemonico_mips) {
    emitir_comentario("Expressao Logica/Relacional");
    gerar_expressao(no->filho1); 
    
    emitir("addiu $sp, $sp, -4");
    emitir("sw $v0, 0($sp)");
    
    gerar_expressao(no->filho2); 
    
    emitir("lw $t1, 0($sp)");
    emitir("addiu $sp, $sp, 4");
    
    char instrucao[50];
    sprintf(instrucao, "%s $v0, $t1, $v0", mnemonico_mips);
    emitir(instrucao);
}