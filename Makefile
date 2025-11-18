CC=gcc 
CFLAGS= -Wall -Wextra -Werror -g -fsanitize=address -Iinclude/
LDFLAGS=-fsanitize=address 
SRCS=$(wildcard ./src/*.c) \
	 $(wildcard ./src/Game/*.c) \
	 $(wildcard ./src/Utils/*.c) \
	 $(wildcard ./src/Display/*.c)\
	 $(wildcard ./src/Menu/*.c)
OBJS=$(SRCS:.c=.o)
TEST=$(wildcard ./tests/*.c)
TESTO=$(TEST:.c=.o)

.PHONY: check

all: $(OBJS) 
	$(CC) $(LDFLAGS) -o tron $^

AI: src/test_IA.o src/Display/Colors.o src/globals.o src/Utils/String.o src/Display/Board.o src/AI/minmax.o 
	$(CC) $(LDFLAGS) -o test $^

re: clean all

clean:
	${RM} ${OBJS}
