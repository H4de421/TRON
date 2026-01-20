CC=gcc 
CFLAGS= -Wall -Wextra -Werror -g -fsanitize=address -Iinclude/
LDFLAGS=-fsanitize=address 
SRCS= ./src/Inputs.c ./src/globals.c  \
	 $(wildcard ./src/Game/*.c) \
	 $(wildcard ./src/Utils/*.c) \
	 $(wildcard ./src/Display/*.c)\
	 $(wildcard ./src/Multiplayer/*.c)\
	 $(wildcard ./src/Menu/*.c)
OBJS=$(SRCS:.c=.o)
TEST=$(wildcard ./tests/*.c)
TESTO=$(TEST:.c=.o)

.PHONY: check

all: main

main: $(OBJS) src/main.o
	$(CC) $(LDFLAGS) -o tron $^

#AI: src/test_IA.o src/Display/Colors.o src/globals.o src/Utils/String.o src/Display/Board.o src/AI/minmax.o 
#	$(CC) $(LDFLAGS) -o test $^

clear_logs:
	${RM} logs/Server.log
	${RM} logs/Client.log

clean:
	${RM} ${OBJS} src/main.o src/client_main.o tron tron_c
