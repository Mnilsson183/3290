#include <stdlib.h>

#ifndef TRON_H
#define TRON_H

typedef unsigned char* size_2_reg;
typedef unsigned char* size_4_reg;
typedef unsigned char* size_word_reg;
typedef unsigned char* size_loc_reg;


struct simpletron {
	size_word_reg* memory;
	u_int8_t pages_size;
	u_int8_t words_size;
	u_int8_t word_size;
	size_word_reg accum;
	u_int32_t instr_counter;
	size_word_reg instr_reg;
	size_word_reg index_reg;
	size_2_reg opCode;
	size_4_reg operand;
};

typedef  enum {
	ACTION_READ = 10,
	ACTION_WRITE,
	ACTION_LOAD = 20,
	ACTION_LOADIM,
	ACTION_LOADX,
	ACTION_LOADIDX,
	ACTION_STORE = 25,
	ACTION_STOREIDX,
	ACTION_ADD = 30,
	ACTION_ADDX,
	ACTION_SUBTRACT,
	ACTION_SUBTRACTX,
	ACTION_DIVIDE,
	ACTION_DIVIDEX,
	ACTION_MULTIPLY,
	ACTION_MULTIPLYX,
	ACTION_INC,
	ACTION_DEC,
	ACTION_BRANCH,
	ACTION_BRANCHNEG,
	ACTION_BRANCHZERO,
	ACTION_SWAP,
	ACTION_HALT,
} SMPL_ACTION;

struct simpletron* init_simpletron(const int pages_size, const int words_size, const int word_size);
void tron_free(struct simpletron* self);
void tron_move(struct simpletron* self, unsigned char* dest, const unsigned char* src);
void tron_move_n(unsigned char* dest, const unsigned char* src, const int n);
void tron_resize_value(unsigned char* dest, const unsigned char* src, const int current_size, const int new_size);
void tron_resize_unsigned_to_signed(unsigned char* dest, const unsigned char* src, const int current_size, const int new_size);
void parse_instruct(struct simpletron* self, unsigned char* instruct);
int char_to_int_n(unsigned char* chars, int size);
unsigned char* tron_get_from_reg(struct simpletron* self, unsigned char* reg);

void tron_read(struct simpletron* self);
void tron_load(struct simpletron* self);
void tron_loadim(struct simpletron* self);
void tron_loadx(struct simpletron* self);
void tron_loadidx(struct simpletron* self);
void tron_store(struct simpletron* self);
void tron_storeidx(struct simpletron* self);
void tron_add(struct simpletron* self);
void tron_addx(struct simpletron* self);
void tron_subtract(struct simpletron* self);
void tron_subtractx(struct simpletron* self);

#endif
