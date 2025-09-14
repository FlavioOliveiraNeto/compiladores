/*
 * Arquivo de teste completo para a linguagem Goianinha.
 * Testa os principais recursos do compilador:
 * - Declarações de variáveis globais e locais
 * - Declaração e chamada de funções (com e sem retorno)
 * - Comandos de escrita, atribuição e nova linha
 * - Estruturas de controle: se-entao-senao e enquanto-execute
 * - Expressões aritméticas, lógicas e de comparação
 * - Escopo de variáveis
 */
programa teste_completo {
    int resultado_final, contador;
    car letra_inicial;

    // Teste de função com parâmetros e retorno
    int calcular_fib(int n) {
        int a, b, c, i;
        a = 0;
        b = 1;
        i = 2;

        // Teste de comando 'enquanto'
        enquanto (i <= n) execute {
            c = a + b;
            a = b;
            b = c;
            i = i + 1;
        }

        retorne b;
    }

    /*
     * Início do bloco principal do programa.
     * Este é um comentário de múltiplas linhas.
    */
    letra_inicial = 'G';
    contador = 10;

    escreva("--- Iniciando Teste Completo do Compilador Goianinha ---");
    novalinha;
    novalinha;

    escreva("Letra inicial do programa: ");
    escreva(letra_inicial);
    novalinha;

    // Teste de chamada de função e atribuição
    resultado_final = calcular_fib(contador);

    escreva("O termo ");
    escreva(contador);
    escreva(" da sequencia de Fibonacci eh: ");
    escreva(resultado_final);
    novalinha;
    novalinha;

    // Teste da estrutura 'se-entao-senao' com expressão complexa
    escreva("Testando escopo e a estrutura se-entao-senao...");
    novalinha;

    se ((resultado_final > 50) && (letra_inicial == 'G')) entao {
        // Teste de declaração de variável local com o mesmo nome de uma global
        int contador; 
        contador = 99; // Deve se referir à variável local, não à global
        
        escreva("Condicao VERDADEIRA. O resultado eh > 50.");
        novalinha;
        escreva("Valor do 'contador' local (dentro do 'se'): ");
        escreva(contador);
        novalinha;
    } senao {
        escreva("Condicao FALSA. O resultado NAO eh > 50.");
        novalinha;
    }

    escreva("Valor do 'contador' global (fora do 'se'): ");
    escreva(contador); // Deve imprimir o valor global (10)
    novalinha;
    novalinha;
    
    // Opcional: Teste do comando 'leia'
    // O compilador irá parar e esperar por um valor inteiro
    escreva("Digite um novo valor para o contador e pressione Enter:");
    novalinha;
    leia contador; 
    escreva("O novo valor do contador eh: ");
    escreva(contador);
    novalinha;

    escreva("--- Fim do Teste ---");
    novalinha;
}