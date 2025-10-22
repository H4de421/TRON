CC=gcc 
CFLAGS= -Wall -Wextra -Werror -g -fsanitize=address -Iinclude/
LDFLAGS=-fsanitize=address 
SRCS=$(wildcard ./src/*.c) 
OBJS=$(SRCS:.c=.o)
TEST=$(wildcard ./tests/*.c)
TESTO=$(TEST:.c=.o)

.PHONY: check

all: $(OBJS) 
	$(CC) $(LDFLAGS) -o main $^

check: $(OBJS) $(TESTO)
	$(LINK.o) $^ -o test #-lcriterion
	./test


clean:
	${RM} -f ${OBJS}
