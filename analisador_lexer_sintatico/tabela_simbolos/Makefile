# Define o compilador C
CC = gcc

# Define as opções de compilação
CFLAGS = -Wall -g

# Define o nome do executável
TARGET = test_tabela_simbolos

# Define os arquivos fonte
SRCS = tabela_simbolos.c main_tabela_simbolos.c

# Define os arquivos objeto
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c tabela_simbolos.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
