#include "modulo_analisador_semantico.h"
#include <stdio.h>

/* * Variáveis de estado 'static' (privadas) para o analisador semântico.
 * Controlam o estado atual da análise.
*/

static int contador_erros_semanticos = 0;

// Ponteiro para a função que está sendo analisada no momento (essencial para o 'retorne')
static NoAST *funcao_atual = NULL;

static void percorrer_ast(NoAST *no);

/**
 * @brief Função auxiliar para reportar um erro semântico.
 * Imprime a mensagem de erro, a linha e incrementa o contador global.
 * @param mensagem A mensagem de erro.
 * @param linha A linha onde o erro ocorreu.
 */
static void reportar_erro(const char* mensagem, int linha) {
    fprintf(stderr, "ERRO SEMANTICO: %s (linha %d)\n", mensagem, linha);
    contador_erros_semanticos++;
}

/**
 * @brief Função auxiliar para percorrer uma lista encadeada da AST.
 * Uma lista é uma sequência de nós ligados pelo ponteiro 'proximo'. (Ex: ListaDeclaracoes, ListaComandos, ListaParametros)
 * @param no A cabeça da lista.
 */
static void percorrer_lista(NoAST *no) {
    NoAST *temp = no;
    while (temp != NULL) {
        percorrer_ast(temp);
        temp = temp->proximo;
    }
}

/**
 * @brief Função recursiva principal que percorre a AST (visitor).
 * É um "dispatcher" que chama a lógica específica para cada TipoNoAST.
 * @param no O nó atual da árvore a ser visitado.
 */
static void percorrer_ast(NoAST *no) {
    if (no == NULL) {
        return; 
    }

    switch (no->tipo_no) {
        case NO_PROGRAMA:
            criar_novo_escopo_e_empilhar();

            percorrer_lista(no->filho1);
            percorrer_lista(no->filho2); 
            remover_escopo_atual();
            break;

        case NO_DECL_FUNCAO:
            EntradaTabela *nova_func = inserir_funcao_na_tabela_atual(
                no->filho1->lexema,
                no->tipo_dado_computado,
                0
            );
            
            funcao_atual = no; 

            criar_novo_escopo_e_empilhar();

            int num_args = 0;
            NoAST *param = no->filho2;
            while (param != NULL) {
                if (param->tipo_no == NO_PARAMETRO) {
                    inserir_parametro_na_tabela_atual(
                        param->filho1->lexema,      
                        param->tipo_dado_computado, 
                        num_args
                    );
                    adicionar_parametro_a_funcao(
                        nova_func,
                        param->filho1->lexema,
                        param->tipo_dado_computado
                    );
                    num_args++;
                }
                param = param->proximo;
            }
            nova_func->num_argumentos = num_args;
            
            percorrer_ast(no->filho3); 
            
            remover_escopo_atual(); 
            funcao_atual = NULL; 
            break;

        case NO_BLOCO:
            int escopo_criado = 0;
            if (funcao_atual == NULL || no != funcao_atual->filho3) {
                 criar_novo_escopo_e_empilhar();
                 escopo_criado = 1;
            }
            
            percorrer_lista(no->filho1); 
            percorrer_lista(no->filho2); 
            
            if (escopo_criado) {
                remover_escopo_atual();
            }
            break;

        case NO_DECL_VARIAVEL:
        {
            if (funcao_atual != NULL && pilha_tabelas->proximo != NULL) {
                EntradaTabela *check = pesquisar_nome_na_pilha(no->filho1->lexema);
                if (check != NULL && check->posicao >= 0) {
                     reportar_erro("Variavel local com o mesmo nome de um parametro.", no->linha);
                }
            }
            
            inserir_variavel_na_tabela_atual(
                no->filho1->lexema,
                no->tipo_dado_computado,
                -1 
            );
            EntradaTabela* entrada = pesquisar_nome_na_pilha(no->filho1->lexema);
            if (entrada != NULL) {
                no->filho1->entrada_tabela = entrada;
            }
            break;
        }

        case NO_COMANDO_ATRIBUICAO:
            percorrer_ast(no->filho1); 
            percorrer_ast(no->filho2); 

            TipoDado tipo_lhs = no->filho1->tipo_dado_computado;
            TipoDado tipo_rhs = no->filho2->tipo_dado_computado;

            if (tipo_lhs == TIPO_VOID || tipo_rhs == TIPO_VOID) {
            } else if (tipo_lhs != tipo_rhs) {
                reportar_erro("Tipos incompativeis na atribuicao.", no->linha);
            }
            no->tipo_dado_computado = tipo_lhs;
            break;

        case NO_COMANDO_SE:
        case NO_COMANDO_ENQUANTO:
            percorrer_ast(no->filho1);

            if (no->filho1->tipo_dado_computado != TIPO_INT) {
                reportar_erro("Expressao de condicao deve ser do tipo 'int'.", no->linha);
            }

            percorrer_ast(no->filho2); 
            percorrer_ast(no->filho3); 
            break;

        case NO_COMANDO_LEIA:
            percorrer_ast(no->filho1);
            if (no->filho1->tipo_dado_computado == TIPO_VOID) {
                reportar_erro("Variavel de 'leia' nao declarada.", no->linha);
            }
            break;

        case NO_COMANDO_ESCREVA:
            if (no->filho1->tipo_no == NO_LITERAL_STRING) {

            } else {

                percorrer_ast(no->filho1);
                if(no->filho1->tipo_dado_computado == TIPO_VOID) {
                    reportar_erro("Nao e possivel 'escrever' uma expressao 'void' ou nao declarada.", no->linha);
                }
            }
            break;

        case NO_COMANDO_RETORNE:
            if (funcao_atual == NULL) {
                reportar_erro("'retorne' utilizado fora de uma funcao.", no->linha);
                break;
            }
            
            TipoDado tipo_esperado = funcao_atual->tipo_dado_computado; 
            
            if (no->filho1 == NULL) { 

                if (tipo_esperado != TIPO_VOID) {
                    reportar_erro("Retorno vazio em funcao que espera um valor.", no->linha);
                }
            } else { 
                percorrer_ast(no->filho1); 
                TipoDado tipo_retornado = no->filho1->tipo_dado_computado;

                if (tipo_esperado == TIPO_VOID) {
                    reportar_erro("Retorno com valor em funcao 'void'.", no->linha);
                } else if (tipo_esperado != tipo_retornado) {
                    reportar_erro("Tipo de retorno incompativel com a declaracao da funcao.", no->linha);
                }
            }
            break;


        case NO_OP_SOMA:
        case NO_OP_SUB:
        case NO_OP_MULT:
        case NO_OP_DIV:
            percorrer_ast(no->filho1); 
            percorrer_ast(no->filho2); 

            if (no->filho1->tipo_dado_computado != TIPO_INT || no->filho2->tipo_dado_computado != TIPO_INT) {
                reportar_erro("Operadores aritmeticos exigem operandos do tipo 'int'.", no->linha);
                no->tipo_dado_computado = TIPO_VOID; 
            } else {
                no->tipo_dado_computado = TIPO_INT; 
            }
            break;
            
        case NO_OP_IGUAL:
        case NO_OP_DIFERENTE:
        case NO_OP_MENOR:
        case NO_OP_MAIOR:
        case NO_OP_MENOR_IGUAL:
        case NO_OP_MAIOR_IGUAL:
            percorrer_ast(no->filho1); 
            percorrer_ast(no->filho2); 
            
            TipoDado tipo_lhs_rel = no->filho1->tipo_dado_computado;
            TipoDado tipo_rhs_rel = no->filho2->tipo_dado_computado;

            if (tipo_lhs_rel == TIPO_VOID || tipo_rhs_rel == TIPO_VOID) {
                no->tipo_dado_computado = TIPO_VOID; 
            } else if (tipo_lhs_rel != tipo_rhs_rel) {
                reportar_erro("Operadores relacionais exigem operandos de tipos iguais.", no->linha);
                no->tipo_dado_computado = TIPO_VOID;
            } else {
                no->tipo_dado_computado = TIPO_INT;
            }
            break;

        case NO_OP_E:
        case NO_OP_OU:
            percorrer_ast(no->filho1); 
            percorrer_ast(no->filho2); 

            if (no->filho1->tipo_dado_computado != TIPO_INT || no->filho2->tipo_dado_computado != TIPO_INT) {
                reportar_erro("Operadores logicos (&&, ||) exigem operandos 'int'.", no->linha);
                no->tipo_dado_computado = TIPO_VOID;
            } else {
                no->tipo_dado_computado = TIPO_INT;
            }
            break;

        case NO_OP_NEGACAO:
            percorrer_ast(no->filho1);
            if (no->filho1->tipo_dado_computado != TIPO_INT) {
                reportar_erro("Operador '!' exige operando 'int'.", no->linha);
                no->tipo_dado_computado = TIPO_VOID;
            } else {
                no->tipo_dado_computado = TIPO_INT;
            }
            break;

        case NO_CHAMADA_FUNCAO:

            EntradaTabela *func = pesquisar_nome_na_pilha(no->filho1->lexema);
            if (func == NULL || func->tipo != TIPO_FUNCAO) {
                reportar_erro("Funcao nao declarada.", no->linha);
                no->tipo_dado_computado = TIPO_VOID; 
                break;
            }
            
            no->filho1->entrada_tabela = func;
            
            ArgumentoFuncao *param_esperado = func->argumentos;
            NoAST *arg_passado = no->filho2; 
            int cont_arg = 1;

            while (param_esperado != NULL && arg_passado != NULL) {
                percorrer_ast(arg_passado);
                
                if (param_esperado->tipo_arg != arg_passado->tipo_dado_computado) {
                    char msg[100];
                    sprintf(msg, "Tipo incorreto para o argumento %d da funcao '%s'.", cont_arg, func->lexema);
                    reportar_erro(msg, arg_passado->linha);
                }
                
                param_esperado = param_esperado->proximo;
                arg_passado = arg_passado->proximo;
                cont_arg++;
            }
            
            if (param_esperado != NULL || arg_passado != NULL) {
                char msg[100];
                sprintf(msg, "Numero incorreto de argumentos para a funcao '%s'.", func->lexema);
                reportar_erro(msg, no->linha);
            }
            
            no->tipo_dado_computado = func->tipo_retorno;
            break;


        case NO_ID:
            EntradaTabela *entrada = pesquisar_nome_na_pilha(no->lexema);
            
            if (entrada == NULL) {
                reportar_erro("Identificador nao declarado.", no->linha);
                no->tipo_dado_computado = TIPO_VOID; 
            } else {
                if (entrada->tipo == TIPO_FUNCAO) {
                    reportar_erro("Uso invalido de nome de funcao (deve ser uma chamada).", no->linha);
                    no->tipo_dado_computado = TIPO_VOID;
                } else {
                    no->tipo_dado_computado = entrada->tipo; 
                }
                no->entrada_tabela = entrada;
            }
            break;

        case NO_LITERAL_INT:
            no->tipo_dado_computado = TIPO_INT;
            break;

        case NO_LITERAL_CAR:
            no->tipo_dado_computado = TIPO_CAR;
            break;

        case NO_LITERAL_STRING:
            no->tipo_dado_computado = TIPO_VOID; 
            break;

        case NO_PARAMETRO:       
        case NO_COMANDO_NOVALINHA: 
            break;
    }
}


/**
 * @brief Função principal (pública) do analisador semântico.
 */
int analisar_semantica(NoAST *raiz) {

    contador_erros_semanticos = 0;
    funcao_atual = NULL;
    
    percorrer_ast(raiz);
    
    return contador_erros_semanticos;
}