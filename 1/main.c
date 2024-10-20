#include "tron.h"
#include <stdlib.h>

#define DEFAULT_PAGES_SIZE 100
#define DEFAULT_WORDS_SIZE 100
#define DEFAULT_WORD_SIZE 7

int main(void) {
	struct simpletron* tron = init_simpletron(DEFAULT_PAGES_SIZE, DEFAULT_WORDS_SIZE, DEFAULT_WORD_SIZE);
	tron_run(tron, "in.smpl");
	tron_move_n(tron->operand, (unsigned char*)"0002", 4);
	tron_halt(tron);
	tron_free(tron);
}
