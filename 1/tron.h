#include <stdlib.h>
#include <sys/_types/_u_int16_t.h>
#include <sys/_types/_u_int8_t.h>

#ifndef TRON_H
#define TRON_H

typedef unsigned char* size_2_reg;
typedef unsigned char* size_4_reg;
typedef unsigned char* size_word_reg;
typedef unsigned char* size_loc_reg;

typedef u_int8_t* bin;
typedef char* hex;

typedef enum{
	TRON_OK,
	TRON_FATAL_ERROR,
	TRON_WARNING,
	TRON_RECOVERABLE_ERROR,
} Simpletron_status;

// pages 0 + 1 are instruction memory
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
	Simpletron_status status;
	char* status_msg;
	u_int16_t flags;
};

//Bit #	Mask	Abbreviation	Description	Category	=1	=0
//0	0x0001	CF	Carry flag	Status	CY (Carry)	NC (No Carry)
//1	0x0002	—	Reserved, always 1 in EFLAGS[2][3]	—
//2	0x0004	PF	Parity flag	Status	PE (Parity Even)	PO (Parity Odd)
//3	0x0008	—	Reserved[3]	—
//4	0x0010	AF	Auxiliary Carry flag[4]	Status	AC (Auxiliary Carry)	NA (No Auxiliary Carry)
//5	0x0020	—	Reserved[3]	—
//6	0x0040	ZF	Zero flag	Status	ZR (Zero)	NZ (Not Zero)
//7	0x0080	SF	Sign flag	Status	NG (Negative)	PL (Positive)
//8	0x0100	TF	Trap flag (single step)	Control
//9	0x0200	IF	Interrupt enable flag	Control	EI (Enable Interrupt)	DI (Disable Interrupt)
//10	0x0400	DF	Direction flag	Control	DN (Down)	UP (Up)
//11	0x0800	OF	Overflow flag	Status	OV (Overflow)	NV (Not Overflow)
//14	0x4000	NT	Nested task flag (286+ only),
//always 1 on 8086 and 186	System

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
	ACTION_UNKNOWN
} SMPL_ACTION;

struct simpletron* init_simpletron(const int pages_size, const int words_size, const int word_size);
void tron_free(struct simpletron* self);
void tron_move(struct simpletron* self, unsigned char* dest, const unsigned char* src);
void tron_move_n(unsigned char* dest, const unsigned char* src, const int n);
void tron_resize_value(unsigned char* dest, const unsigned char* src, const int current_size, const int new_size);
void tron_resize_unsigned_to_signed(unsigned char* dest, const unsigned char* src, const int current_size, const int new_size);
void parse_instruct(struct simpletron* self, unsigned char* instruct);
int char_to_int_n(unsigned char* chars, int size);
unsigned char* int_to_chars_known_size(int val, int val_size, unsigned char* dest, int dest_size);
unsigned char* tron_get_from_word(struct simpletron* self, size_loc_reg reg);

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

unsigned char* tron_add_words(struct simpletron* self, unsigned char* c1, unsigned char* c2, unsigned char* d);
unsigned char* tron_sub_words(struct simpletron* self, unsigned char* c1, unsigned char* c2, unsigned char* d);
unsigned char* tron_mul_words(struct simpletron* self, unsigned char* c1, unsigned char* c2, unsigned char* d);
unsigned char* tron_mod_words(struct simpletron* self, unsigned char* c1, unsigned char* c2, unsigned char* d);
unsigned char* tron_div_words(struct simpletron* self, unsigned char* c1, unsigned char* c2, unsigned char* d);

#endif
