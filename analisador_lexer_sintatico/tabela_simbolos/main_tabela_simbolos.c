// Programa de teste para a implementação da pilha de tabelas de símbolos.
#include "tabela_simbolos.h"
#include <stdio.h>

int main() {
    printf("--- Teste da Pilha de Tabelas de Simbolos ---\n");

    iniciar_pilha_tabela_simbolos();

    criar_novo_escopo_e_empilhar();
    imprimir_tabela_atual();

    inserir_variavel_na_tabela_atual("x", TIPO_INT, 0);
    inserir_variavel_na_tabela_atual("nomePrograma", TIPO_CAR, 1);
    imprimir_tabela_atual();

    EntradaTabela *func_main = inserir_funcao_na_tabela_atual("main", TIPO_VOID, 0);
    imprimir_tabela_atual();

    EntradaTabela *encontrado = pesquisar_nome_na_pilha("x");
    if (encontrado) {
        printf("Pesquisado 'x': Encontrado (Lexema: %s, Tipo: %d).\n", encontrado->lexema, encontrado->tipo);
    } else {
        printf("Pesquisado 'x': Nao encontrado.\n");
    }

    encontrado = pesquisar_nome_na_pilha("naoExiste");
    if (encontrado) {
        printf("Pesquisado 'naoExiste': Encontrado.\n");
    } else {
        printf("Pesquisado 'naoExiste': Nao encontrado.\n");
    }

    criar_novo_escopo_e_empilhar();
    imprimir_tabela_atual();

    inserir_variavel_na_tabela_atual("temp", TIPO_INT, 0);
    inserir_parametro_na_tabela_atual("arg1", TIPO_INT, 1);
    inserir_parametro_na_tabela_atual("arg2", TIPO_CAR, 2);
    imprimir_tabela_atual();

    EntradaTabela *func_soma = inserir_funcao_na_tabela_atual("soma", TIPO_INT, 2);
    if (func_soma) {
        adicionar_parametro_a_funcao(func_soma, "a", TIPO_INT);
        adicionar_parametro_a_funcao(func_soma, "b", TIPO_INT);
    }

    encontrado = pesquisar_nome_na_pilha("temp");
    if (encontrado) {
        printf("Pesquisado 'temp': Encontrado (Lexema: %s, Tipo: %d).\n", encontrado->lexema, encontrado->tipo);
    } else {
        printf("Pesquisado 'temp': Nao encontrado.\n");
    }

    encontrado = pesquisar_nome_na_pilha("x");
    if (encontrado) {
        printf("Pesquisado 'x': Encontrado (Lexema: %s, Tipo: %d).\n", encontrado->lexema, encontrado->tipo);
    } else {
        printf("Pesquisado 'x': Nao encontrado.\n");
    }

    criar_novo_escopo_e_empilhar();
    imprimir_tabela_atual();
    inserir_variavel_na_tabela_atual("i", TIPO_INT, 0);
    imprimir_tabela_atual();

    remover_escopo_atual();
    imprimir_tabela_atual();

    encontrado = pesquisar_nome_na_pilha("i");
    if (encontrado) {
        printf("Pesquisado 'i': Encontrado.\n");
    } else {
        printf("Pesquisado 'i': Nao encontrado (correto).\n");
    }

    remover_escopo_atual();
    imprimir_tabela_atual();

    remover_escopo_atual();
    imprimir_tabela_atual();

    remover_escopo_atual();

    eliminar_pilha_tabela_simbolos();

    printf("--- Fim do Teste ---\n");

    return 0;
}
