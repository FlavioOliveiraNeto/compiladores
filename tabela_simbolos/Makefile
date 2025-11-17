CC = gcc
CFLAGS = -Wall -g
TARGET = test_tabela_simbolos
SRCS = tabela_simbolos.c main_tabela_simbolos.c
OBJS = $(SRCS:.c=.o)
.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
%.o: %.c tabela_simbolos.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)