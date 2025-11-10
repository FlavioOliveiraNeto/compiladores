	.data
newline:
	.asciiz "\n"
	
.text
	.globl main

	# --- Declaracoes de Funcoes ---

	# --- Programa Principal (main) ---
main:
	move $fp, $sp

	# AVISO: Expressao encontrada fora de contexto de comando.

	# --- Fim do Programa (exit) ---
	li $v0, 10
	syscall
