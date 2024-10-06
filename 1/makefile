CC=gcc
MAIN_FILE = main.c
SRC_FILES = tron.c
TEST_FILES = test.c assert.c
FLAGS = -Wall -Wextra -Werror -g
EXEC = a.out

all:
	${CC} ${FLAGS} ${SRC_FILES} ${MAIN_FILE} -o ${EXEC}
	@echo 'builing to ${EXEC}'
run: all
	./a.out
clean:
	rm ${EXEC}

