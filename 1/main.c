#include "tron.h"
#include <stdlib.h>

#define DEFAULT_PAGES_SIZE 100
#define DEFAULT_WORDS_SIZE 100
#define DEFAULT_WORD_SIZE 7

int main(void) {
	struct simpletron* tron = init_simpletron(DEFAULT_PAGES_SIZE, DEFAULT_WORDS_SIZE, DEFAULT_WORD_SIZE);
}
