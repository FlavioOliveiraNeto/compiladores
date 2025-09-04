//Arquivo que contém a implementação das funções para a pilha de tabelas de símbolos, com comentários para explicar o que cada uma faz.

#include "tabela_simbolos.h"

TabelaSimbolos *pilha_tabelas = NULL;

/**
 * @brief Inicializa a pilha de tabelas de símbolos, tornando-a vazia.
 */
void iniciar_pilha_tabela_simbolos() {
    pilha_tabelas = NULL; 
    printf("Pilha de tabelas de simbolos inicializada.\n");
}

/**
 * @brief Cria uma nova tabela de símbolos (um novo escopo) e a empilha no topo.
 */
void criar_novo_escopo_e_empilhar() {
    TabelaSimbolos *nova_tabela = (TabelaSimbolos *)malloc(sizeof(TabelaSimbolos));
    if (nova_tabela == NULL) {
        perror("Erro ao alocar memoria para nova tabela de simbolos");
        exit(EXIT_FAILURE);
    }
    nova_tabela->primeira_entrada = NULL; 
    nova_tabela->proximo = pilha_tabelas; 
    pilha_tabelas = nova_tabela;       
    printf("Novo escopo criado e empilhado.\n");
}

/**
 * @brief Pesquisa um lexema (nome) na pilha de tabelas de símbolos,
 * começando pelo escopo atual (topo) e descendo.
 * @param nome O lexema a ser pesquisado.
 * @return Um ponteiro para a EntradaTabela encontrada ou NULL se não encontrar.
 */
EntradaTabela *pesquisar_nome_na_pilha(const char *nome) {
    TabelaSimbolos *escopo_atual = pilha_tabelas;
    while (escopo_atual != NULL) {
        EntradaTabela *entrada_atual = escopo_atual->primeira_entrada;
        while (entrada_atual != NULL) {
            if (strcmp(entrada_atual->lexema, nome) == 0) {
                return entrada_atual; 
            }
            entrada_atual = entrada_atual->proximo;
        }
        escopo_atual = escopo_atual->proximo;
    }
    return NULL;
}

/**
 * @brief Remove a tabela de símbolos que está no topo da pilha, encerrando o escopo atual.
 */
void remover_escopo_atual() {
    if (pilha_tabelas == NULL) {
        printf("Erro: Pilha de tabelas vazia. Nao ha escopo para remover.\n");
        return;
    }

    TabelaSimbolos *escopo_a_remover = pilha_tabelas;
    pilha_tabelas = pilha_tabelas->proximo; 

    EntradaTabela *entrada_atual = escopo_a_remover->primeira_entrada;
    while (entrada_atual != NULL) {
        EntradaTabela *temp_entrada = entrada_atual;
        entrada_atual = entrada_atual->proximo;
        free(temp_entrada->lexema); 
        
        if (temp_entrada->tipo == TIPO_FUNCAO) {
            ArgumentoFuncao *arg_atual = temp_entrada->argumentos;
            while (arg_atual != NULL) {
                ArgumentoFuncao *temp_arg = arg_atual;
                arg_atual = arg_atual->proximo;
                free(temp_arg->nome_arg);
                free(temp_arg);
            }
        }
        free(temp_entrada);
    }
    free(escopo_a_remover);
    printf("Escopo atual removido.\n");
}

/**
 * @brief Cria uma nova entrada na tabela de símbolos atual.
 * @param lexema O nome do identificador.
 * @param tipo O tipo do identificador.
 * @param posicao A posição do identificador.
 * @return Um ponteiro para a EntradaTabela criada.
 */
static EntradaTabela *criar_entrada(const char *lexema, TipoDado tipo, int posicao) {
    EntradaTabela *nova_entrada = (EntradaTabela *)malloc(sizeof(EntradaTabela));
    if (nova_entrada == NULL) {
        perror("Erro ao alocar memoria para entrada da tabela de simbolos");
        exit(EXIT_FAILURE);
    }
    nova_entrada->lexema = strdup(lexema);
    if (nova_entrada->lexema == NULL) {
        perror("Erro ao alocar memoria para lexema");
        free(nova_entrada);
        exit(EXIT_FAILURE);
    }
    nova_entrada->tipo = tipo;
    nova_entrada->posicao = posicao;
    nova_entrada->num_argumentos = 0;
    nova_entrada->tipo_retorno = TIPO_VOID;
    nova_entrada->argumentos = NULL;
    nova_entrada->proximo = NULL;
    return nova_entrada;
}

/**
 * @brief Insere um nome de função na tabela de símbolos atual (topo da pilha).
 * @param nome_funcao O lexema da função.
 * @param tipo_retorno O tipo de retorno da função (TIPO_INT, TIPO_CAR, TIPO_VOID).
 * @param num_argumentos O número de argumentos da função.
 * @return Um ponteiro para a EntradaTabela da função.
 */
EntradaTabela *inserir_funcao_na_tabela_atual(const char *nome_funcao, TipoDado tipo_retorno, int num_argumentos) {
    if (pilha_tabelas == NULL) {
        printf("Erro: Pilha de tabelas vazia. Nao ha escopo atual para inserir.\n");
        return NULL;
    }

    EntradaTabela *temp_entrada = pilha_tabelas->primeira_entrada;
    while (temp_entrada != NULL) {
        if (strcmp(temp_entrada->lexema, nome_funcao) == 0 && temp_entrada->tipo == TIPO_FUNCAO) {
            printf("Aviso: Funcao '%s' ja declarada neste escopo.\n", nome_funcao);
            return temp_entrada;
        }
        temp_entrada = temp_entrada->proximo;
    }

    EntradaTabela *nova_entrada = criar_entrada(nome_funcao, TIPO_FUNCAO, -1); 
    nova_entrada->tipo_retorno = tipo_retorno;
    nova_entrada->num_argumentos = num_argumentos;
    nova_entrada->proximo = pilha_tabelas->primeira_entrada;
    pilha_tabelas->primeira_entrada = nova_entrada;
    printf("Funcao '%s' (retorno: %d, args: %d) inserida no escopo atual.\n", nome_funcao, tipo_retorno, num_argumentos);
    return nova_entrada;
}

/**
 * @brief Insere um parâmetro na lista de argumentos de uma entrada de função.
 * @param funcao_entrada A entrada da tabela de símbolos correspondente à função.
 * @param nome_parametro O nome do parâmetro.
 * @param tipo_parametro O tipo do parâmetro.
 */
void adicionar_parametro_a_funcao(EntradaTabela *funcao_entrada, const char *nome_parametro, TipoDado tipo_parametro) {
    if (funcao_entrada == NULL || funcao_entrada->tipo != TIPO_FUNCAO) {
        printf("Erro: Tentativa de adicionar parametro a uma entrada que nao e funcao ou e invalida.\n");
        return;
    }

    ArgumentoFuncao *novo_arg = (ArgumentoFuncao *)malloc(sizeof(ArgumentoFuncao));
    if (novo_arg == NULL) {
        perror("Erro ao alocar memoria para argumento de funcao");
        exit(EXIT_FAILURE);
    }
    novo_arg->nome_arg = strdup(nome_parametro);
    if (novo_arg->nome_arg == NULL) {
        perror("Erro ao alocar memoria para nome do argumento");
        free(novo_arg);
        exit(EXIT_FAILURE);
    }
    novo_arg->tipo_arg = tipo_parametro;
    novo_arg->proximo = NULL;

    if (funcao_entrada->argumentos == NULL) {
        funcao_entrada->argumentos = novo_arg;
    } else {
        ArgumentoFuncao *temp_arg = funcao_entrada->argumentos;
        while (temp_arg->proximo != NULL) {
            temp_arg = temp_arg->proximo;
        }
        temp_arg->proximo = novo_arg;
    }
    printf("Parametro '%s' (tipo: %d) adicionado a funcao '%s'.\n", nome_parametro, tipo_parametro, funcao_entrada->lexema);
}

/**
 * @brief Insere uma variável na tabela de símbolos atual (topo da pilha).
 * @param nome_variavel O lexema da variável.
 * @param tipo_variavel O tipo da variável (TIPO_INT, TIPO_CAR).
 * @param posicao A posição da variável na declaração.
 */
void inserir_variavel_na_tabela_atual(const char *nome_variavel, TipoDado tipo_variavel, int posicao) {
    if (pilha_tabelas == NULL) {
        printf("Erro: Pilha de tabelas vazia. Nao ha escopo atual para inserir.\n");
        return;
    }

    EntradaTabela *temp_entrada = pilha_tabelas->primeira_entrada;
    while (temp_entrada != NULL) {
        if (strcmp(temp_entrada->lexema, nome_variavel) == 0 && temp_entrada->tipo != TIPO_FUNCAO) {
            printf("Aviso: Variavel '%s' ja declarada neste escopo.\n", nome_variavel);
            return;
        }
        temp_entrada = temp_entrada->proximo;
    }

    EntradaTabela *nova_entrada = criar_entrada(nome_variavel, tipo_variavel, posicao);

    nova_entrada->proximo = pilha_tabelas->primeira_entrada;
    pilha_tabelas->primeira_entrada = nova_entrada;
    printf("Variavel '%s' (tipo: %d, pos: %d) inserida no escopo atual.\n", nome_variavel, tipo_variavel, posicao);
}

/**
 * @brief Insere um parâmetro de função na tabela de símbolos atual (topo da pilha).
 * Nota: Os parâmetros também são variáveis locais no escopo da função.
 * @param nome_parametro O lexema do parâmetro.
 * @param tipo_parametro O tipo do parâmetro (TIPO_INT, TIPO_CAR).
 * @param posicao A posição do parâmetro na declaração.
 */
void inserir_parametro_na_tabela_atual(const char *nome_parametro, TipoDado tipo_parametro, int posicao) {
    inserir_variavel_na_tabela_atual(nome_parametro, tipo_parametro, posicao);
    printf("Parametro '%s' (tipo: %d, pos: %d) inserido no escopo atual.\n", nome_parametro, tipo_parametro, posicao);
}

/**
 * @brief Elimina toda a pilha de tabelas de símbolos e libera a memória alocada.
 */
void eliminar_pilha_tabela_simbolos() {
    while (pilha_tabelas != NULL) {
        remover_escopo_atual(); 
    }
    printf("Pilha de tabelas de simbolos eliminada e memoria liberada.\n");
}

/**
 * @brief Função auxiliar para imprimir o conteúdo da tabela de símbolos atual.
 */
void imprimir_tabela_atual() {
    if (pilha_tabelas == NULL) {
        printf("Pilha de tabelas vazia.\n");
        return;
    }
    printf("--- Conteudo do Escopo Atual ---\n");
    EntradaTabela *entrada_atual = pilha_tabelas->primeira_entrada;
    if (entrada_atual == NULL) {
        printf("Escopo atual esta vazio.\n");
    }
    while (entrada_atual != NULL) {
        printf("  Lexema: %s, Tipo: %d, Posicao: %d", entrada_atual->lexema, entrada_atual->tipo, entrada_atual->posicao);
        if (entrada_atual->tipo == TIPO_FUNCAO) {
            printf(", Retorno: %d, Args: %d\n", entrada_atual->tipo_retorno, entrada_atual->num_argumentos);
            ArgumentoFuncao *arg_atual = entrada_atual->argumentos;
            while (arg_atual != NULL) {
                printf("    - Parametro: %s, Tipo: %d\n", arg_atual->nome_arg, arg_atual->tipo_arg);
                arg_atual = arg_atual->proximo;
            }
        } else {
            printf("\n");
        }
        entrada_atual = entrada_atual->proximo;
    }
    printf("-------------------------------\n");
}
