#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_u_int16_t.h>
#include <sys/_types/_u_int32_t.h>
#include <sys/_types/_u_int8_t.h>
#include <sys/_types/_uid_t.h>
#include <math.h>
#include <strings.h>
#include <string.h>
#include "tron.h"

#define INT_LENGTH(x) ((x) == 0 ? 1 : (int)(log10(abs(x)) + 1))

void tron_handle_error(struct simpletron* self);

struct simpletron* init_simpletron(const int pages_size, const int words_size, const int word_size) {
	struct simpletron* tron = malloc(sizeof(struct simpletron));
	if (tron == NULL) return NULL;

	tron->pages_size = pages_size;
	tron->words_size = words_size;
	tron->word_size = word_size;
	tron->accum = (unsigned char*)calloc(1, words_size);
	tron->instr_counter = 0;
	tron->instr_reg = (unsigned char*)calloc(1, words_size);
	tron->index_reg = (unsigned char*)calloc(1, words_size);
	
	tron->memory = malloc(tron->pages_size * sizeof(unsigned char *));
	for (int i = 0; i < tron->pages_size; i++) {
		tron->memory[i] = (unsigned char*)calloc(tron->words_size, words_size);
	}

	tron->opCode = (unsigned char*)calloc(1, 2);
	tron->operand = (unsigned char*)calloc(1, 4);
	tron->flags = 0;
	return tron;
}

void tron_free(struct simpletron* self) {
	for (int i = 0; i < self->pages_size; i++) {
		free(self->memory[i]);
	}
	free(self->memory);
	free(self->operand);
	free(self->opCode);
	free(self->accum);
	free(self->instr_reg);
	free(self->index_reg);
	free(self);
}

void tron_set_flag(struct simpletron* self, int flag) {
	self->flags = self->flags | 1 << flag;
}

void tron_toggle_flag(struct simpletron* self, int flag) {
	self->flags = self->flags ^ 1 << flag;
}

void tron_clear_flag(struct simpletron* self, int flag) {
	self->flags = self->flags & ~(1 << flag);
}

int tron_get_flag(struct simpletron* self, int flag) {
	return self->flags & 1 << flag;
}

void tron_move(struct simpletron* self, unsigned char* dest, const unsigned char* src) {
	for (int i = 0; i < self->word_size; i++) {
		dest[i] = src[i];
	}
}

int tron_mem_equal(const unsigned char* s1, const char* s2, int n) {
	for (int i = 0; i < n; i++) {
		if (s1[i] != s2[i]) return 0;
	}
	return 1;
}

// intended to move into the operand and op code
void tron_move_n(unsigned char* dest, const unsigned char* src, const int n) {
	for (int i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

void tron_resize_value(unsigned char* dest, const unsigned char* src, const int current_size, const int new_size) {
	int diff = current_size - new_size;
	if (diff < new_size) {
		// copy sign
		int idx = 1;
		dest[0] = src[0];
		for (int i = 0; i < diff * -1; i++) {
			dest[idx] = '0';
			idx++;
		}
		for (int i = 1; i < current_size; i++) {
			dest[idx] = src[i];
			idx++;

		}
	}
}

void tron_resize_unsigned_to_signed(unsigned char* dest, const unsigned char* src, const int current_size, const int new_size) {
	int diff = current_size - new_size;
	if (diff < 0) {
		int idx = 1;
		dest[0] = '+';
		for (int i = 0; i < diff * -1 - 1; i++) {
			dest[idx] = '0';
			idx++;
		}
		for (int i = 0; i < current_size; i++) {
			dest[idx] = src[i];
			idx++;
		}
	}
}

void load_instr_set(struct simpletron* self, const char* filename) {
	if (filename == NULL) return;
	FILE* f = fopen(filename, "r");
	char buf[8];
	buf[0] = 0;
	self->instr_counter = 0;
	while(fgets(&buf[1], sizeof(buf)-1, f)) {
		fgetc(f);		// remove the newline char
		memmove(&self->memory[self->instr_counter / 100][(self->instr_counter % 100) * self->word_size], (unsigned char*)buf, 7);
		self->instr_counter++;
	}
	// write the end of the instruction indicator
	memcpy(&self->memory[self->instr_counter / 100][(self->instr_counter % 100) * self->word_size], "9999999", 7);
	self->instr_counter = 0;
}

void parse_instruct(struct simpletron* self, size_instruct instruct) {
	// first 2 op code
	self->opCode[0] = instruct[0];
	self->opCode[1] = instruct[1];
	// last 4 opperand
	self->operand[0] = instruct[2];
	self->operand[1] = instruct[3];
	self->operand[2] = instruct[4];
	self->operand[3] = instruct[5];
}

int char_to_int_n(unsigned char* chars, int size) {
	int sum = 0;
	int i = chars[0] == '-' || chars[0] == '+' ? 1 : 0;
	for (; i < size; i++) {
		sum += (chars[i] - 48) * pow(10, size - i - 1);
	}
	if (chars[0] == '-') return -1 * sum;
	return sum;
}

unsigned char* int_to_chars_known_size(int val, int val_size, unsigned char* dest, int dest_size) {
	unsigned char* chars = malloc(sizeof(unsigned char*) * 7);
	if (val < 0) chars[0] = '-';
	else chars[0] = '+';
	int idx = 1;

	val = abs(val);
	while(idx < val_size + 1) {
		int rem = val % 10;
		val /= 10;
		chars[val_size - idx + 1] = rem + '1' - 1;
		idx++;
	}
	tron_resize_value(dest, chars, val_size + 1, dest_size);
	free(chars);
	return dest;
}

unsigned char* tron_get_from_word(struct simpletron* self, size_loc_reg reg) {
	int page = char_to_int_n(&reg[0], 2);
	int word = char_to_int_n(&reg[2], 2);
	return &self->memory[page][word * self->word_size];
}

unsigned char* tron_get_from_oper(struct simpletron* self) {
	return tron_get_from_word(self, self->operand);
}

void exec_inst(struct simpletron* self, unsigned char* instr) {
	parse_instruct(self, &instr[1]);
	switch ((SMPL_ACTION)char_to_int_n(self->opCode, 2)) {
		case ACTION_READ:
			// TODO
			break;
		case ACTION_WRITE:
			return tron_write(self);

		case ACTION_LOAD:
			return tron_load(self);
		case ACTION_LOADIM:
			return tron_loadim(self);
		case ACTION_LOADX:
			return tron_loadx(self);
		case ACTION_LOADIDX:
			return tron_loadidx(self);

		case ACTION_STORE:
			return tron_store(self);
		case ACTION_STOREIDX:
			return tron_storeidx(self);
		
		case ACTION_ADD:
			return tron_add(self);
		case ACTION_ADDX:
			return tron_addx(self);
		
		case ACTION_SUBTRACT:
			return tron_subtract(self);
		case ACTION_SUBTRACTX:
			return tron_subtractx(self);

		case ACTION_DIVIDE:
			return tron_divide(self);
		case ACTION_DIVIDEX:
			return tron_dividex(self);

		case ACTION_MULTIPLY:
			return tron_multiply(self);
		case ACTION_MULTIPLYX:
			return tron_multiplyx(self);

		case ACTION_INC:
			return tron_inc(self);
		case ACTION_DEC:
			return tron_dec(self);

		case ACTION_BRANCH:
			return tron_branch(self);
		case ACTION_BRANCHNEG:
			return tron_branchneg(self);
		case ACTION_BRANCHZERO:
			return tron_branchzero(self);
		
		case ACTION_SWAP:
			return tron_swap(self);
		case ACTION_HALT:
			return tron_halt(self);
		case ACTION_UNKNOWN:
			tron_handle_error(self);
	}
	if (self->status_msg != TRON_OK) tron_handle_error(self);
}

void tron_run(struct simpletron* self, const char* filename) {
	load_instr_set(self, filename);
	while (!tron_mem_equal(&self->memory[self->instr_counter / 100][(self->instr_counter % 100) * self->word_size], "9999999", self->word_size)) {
		exec_inst(self, &self->memory[self->instr_counter / 100][(self->instr_counter % 100) * self->word_size]);
		self->instr_counter++;
	}
}

unsigned char* tron_add_words(struct simpletron* self, unsigned char* d, unsigned char* c1, unsigned char* c2) {
	int i1 = char_to_int_n(c1, self->word_size);
	int i2 = char_to_int_n(c2, self->word_size);
	int res = i1 + i2;
	return int_to_chars_known_size(res, INT_LENGTH(res), d, self->word_size);
}
unsigned char* tron_sub_words(struct simpletron* self, unsigned char* d, unsigned char* c1, unsigned char* c2) {
	int i1 = char_to_int_n(c1, self->word_size);
	int i2 = char_to_int_n(c2, self->word_size);
	int res = i1 - i2;
	return int_to_chars_known_size(res, INT_LENGTH(res), d, self->word_size);
}
unsigned char* tron_mul_words(struct simpletron* self, unsigned char* d, unsigned char* c1, unsigned char* c2) {
	int i1 = char_to_int_n(c1, self->word_size);
	int i2 = char_to_int_n(c2, self->word_size);
	int res = i1 * i2;
	return int_to_chars_known_size(res, INT_LENGTH(res), d, self->word_size);
}
unsigned char* tron_mod_words(struct simpletron* self, unsigned char* d, unsigned char* c1, unsigned char* c2) {
	int i1 = char_to_int_n(c1, self->word_size);
	int i2 = char_to_int_n(c2, self->word_size);
	if (i2 == 0) {
		self->status = TRON_FATAL_ERROR;
		self->status_msg = "Divide by zero";
		return NULL;
	}
	int res = i1 % i2;
	return int_to_chars_known_size(res, INT_LENGTH(res), d, self->word_size);
}
unsigned char* tron_div_words(struct simpletron* self, unsigned char* d, unsigned char* c1, unsigned char* c2) {
	int i1 = char_to_int_n(c1, self->word_size);
	int i2 = char_to_int_n(c2, self->word_size);
	if (i2 == 0) {
		self->status = TRON_FATAL_ERROR;
		self->status_msg = "Divide by zero";
		return NULL;
	}
	int res = i1 / i2;
	return int_to_chars_known_size(res, INT_LENGTH(res), d, self->word_size);
}

void tron_print_mem_loc(struct simpletron* self, unsigned char* val) {
	for (int i = 0; i < self->word_size; i++) {
		if (val[0] == '-' || val[0] == '+') printf("%c", val[i]);
		else if (val[i] == 0) printf("0");
		else printf("%d", val[i]-48);
	}
	printf(" ");
}

void tron_print_mem_loc_n(unsigned char* val, int n) {
	for (int i = 0; i < n; i++) {
		if (val[0] == '-' || val[0] == '+') printf("%c", val[i]);
		else if (val[i] == 0) printf("0");
		else printf("%d", val[i]-48);
	}
	printf(" ");
}

void tron_dump_page(struct simpletron* self, int n) {
	printf("\n\nPage: %d", n);
	for (int i = 0; i < self->pages_size; i++) {
		if (i % 10 == 0) printf("\n");
		tron_print_mem_loc(self, &self->memory[n][i * self->word_size]);
	}
}

void tron_dump(struct simpletron* self) {
	printf("Accum: ");
	tron_print_mem_loc(self, self->accum);
	printf("\nIndex: ");
	tron_print_mem_loc(self, self->index_reg);
	for (int i = 0; i < self->pages_size; i++) {
		tron_dump_page(self, i);
	}
	printf("\n");
}

void tron_terminate(const char* msg, int errorCode, struct simpletron* tron) {
	tron_dump(tron);
	tron_free(tron);
	printf("%s\nThe Simpletron has exited with code %d", msg, errorCode);
	exit(errorCode);
}

void tron_handle_error(struct simpletron* self) {
	Simpletron_status error = self->status;
	switch (error) {
		case TRON_OK: return; // should never happen
		case TRON_FATAL_ERROR:
			tron_terminate("Fatal Error", TRON_FATAL_ERROR, self); return;
		case TRON_WARNING:
			printf("Warning: %s", self->status_msg == NULL ? "Unknown" : self->status_msg);
			return;
		case TRON_RECOVERABLE_ERROR:
			printf("Recoverable Error: %s", self->status_msg == NULL ? "Unknown" : self->status_msg);
			return;
	}
}

// --------------------------------INSTRUCTION-------------------------------//


//int READ=10 Read a word from the terminal into a location whose address is the operand
//void tron_read(struct simpletron* self) {
//}
//int WRITE=11 Write a word from the location whose address is the operand to the terminal
void tron_write(struct simpletron* self) {
	tron_print_mem_loc(self, tron_get_from_oper(self));

}
//int LOAD=20 Load a word from the memory location specified by the operand into the accumlator
void tron_load(struct simpletron* self) {
	tron_move(self, self->accum, tron_get_from_oper(self));
}
//int LOADIM=21 Load the operand into the accumulator
void tron_loadim(struct simpletron* self) {
	tron_resize_unsigned_to_signed(self->accum, self->operand, 4, self->word_size);
}
//int LOADX=22 Load word from the memory location specified by the operand into the index register
void tron_loadx(struct simpletron* self) {
	tron_move(self, self->index_reg, tron_get_from_oper(self));
}
//int LOADIDX=22 Load word from the memory location specified by index register into accumulator
void tron_loadidx(struct simpletron* self) {
	tron_move(self, self->accum, tron_get_from_word(self, self->index_reg));
}
//int STORE=25 Store a word from the acccumulator into the memory location specified by the operand
void tron_store(struct simpletron* self) {
	tron_move(self, tron_get_from_oper(self), self->accum);
}
//int STOREIDX=26 store a word from the accumulator into a memory location specified by index register
void tron_storeidx(struct simpletron* self) {
	// reading a memory location 4 from a 7 register
	tron_move(self, tron_get_from_word(self, &self->index_reg[3]), self->accum);
}
//int ADD=30 Add the word in memory whose address is the operand to the accumulator and leave result in accumulator ( ACC += MEM )
void tron_add(struct simpletron* self) {
	tron_add_words(self, self->accum, tron_get_from_word(self, self->operand), self->accum);
}
//int ADDX=31 Add a word in memory whose address is stored in index register to the accumulator and leave result in accumulator
void tron_addx(struct simpletron* self) {
	tron_add_words(self, self->accum, tron_get_from_word(self, &self->index_reg[3]), self->accum);
}
//int SUBTRACT=32 Subtract a word whose address stored in the operand from the accumulator and leave result in accumulator ( ACC -= MEM )
void tron_subtract(struct simpletron* self) {
	tron_sub_words(self, self->accum, tron_get_from_word(self, self->operand), self->accum);
}
//int SUBTRACTX=33 Subtract a word whose address is stored in the index register from the accumulator and leave result in accumulator
void tron_subtractx(struct simpletron* self) {
	tron_sub_words(self, self->accum, tron_get_from_word(self, &self->index_reg[3]), self->accum);
}
//int DIVIDE=34 Divide the accumulator by a word whose address stored in the operand and leave result in accumulator and lose the remainder.( ACC /= MEM )
void tron_divide(struct simpletron* self) {
	tron_div_words(self, self->accum, tron_get_from_word(self, self->operand), self->accum);
}
//int DIVIDEX=35 Divide the accumulator by a word whose address is stored in the index register and leave result in accumulator and lose the remainder.
void tron_dividex(struct simpletron* self) {
	tron_div_words(self, self->accum, tron_get_from_word(self, &self->index_reg[3]), self->accum);
}
//int MULTIPLY=36 Multiply the accumulator by a word from a specific location in memory and leave result in accumulator ( ACC *= MEM )
void tron_multiply(struct simpletron* self) {
	tron_mul_words(self, self->accum, tron_get_from_word(self, self->operand), self->accum);
}
//int MULTIPLYX=37 Multiply the accumulator by a word whose address is stored in the index register and leave result in accumulator
void tron_multiplyx(struct simpletron* self) {
	tron_mul_words(self, self->accum, tron_get_from_word(self, &self->index_reg[3]), self->accum);

}
//int INC=38 increase index register by 1
void tron_inc(struct simpletron* self) {
	tron_add_words(self, self->index_reg, self->index_reg, (unsigned char*)"+000001");
}
//int DEC=39 decrease index register by 1
void tron_dec(struct simpletron* self) {
	tron_add_words(self, self->index_reg, self->index_reg, (unsigned char*)"-000001");

}
//int BRANCH=40 Branch to a specific location in memory, location address is in operand
void tron_branch(struct simpletron* self) {
}
//int BRANCHNEG=41 Branch to a specific location in memory if accumulator is negative
void tron_branchneg(struct simpletron* self) {

}
//int BRANCHZERO=42 Branch to a specific location in memory if the accumulator is zero
void tron_branchzero(struct simpletron* self) {

}
//int SWAP=43 swap contents of index register and accumulator
void tron_swap(struct simpletron* self) {
	unsigned char* tmp = malloc(sizeof(unsigned char) * 7);
	tron_move(self, tmp, self->index_reg);
	tron_move(self, self->index_reg, self->accum);
	tron_move(self, self->accum, tmp);
}
//int HALT=45 halt program dump register values and a range of pages. The starting page of the range is stored as the top 2 digits of the operand and the last page as the least significant 2 digits( core dump ).
void tron_halt(struct simpletron* self) {
	printf("\nopCode: ");
	tron_print_mem_loc_n(self->opCode, 2);

	printf("	operand: ");
	tron_print_mem_loc_n(self->operand, 4);

	printf("\nAcc: ");
	tron_print_mem_loc(self, self->accum);

	printf("	idx: ");
	tron_print_mem_loc(self, self->index_reg);

	printf("\n");
	int low = char_to_int_n(&self->operand[0], 2);
	int high = char_to_int_n(&self->operand[2], 2);
	for (int i = low; i < high + 1; i++) {
		tron_dump_page(self, i);
	}
}
