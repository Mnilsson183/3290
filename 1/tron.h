#include <stdlib.h>
#include <sys/_types/_u_int16_t.h>
#include <sys/_types/_u_int8_t.h>

#ifndef TRON_H
#define TRON_H

typedef unsigned char* size_2_reg;
typedef unsigned char* size_4_reg;
typedef unsigned char* size_word_reg;
typedef unsigned char* size_loc_reg;
typedef unsigned char* size_instruct;

typedef enum {
	TRON_OK,
	TRON_FATAL_ERROR,
	TRON_WARNING,
	TRON_RECOVERABLE_ERROR,
} Simpletron_status;

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
	ACTION_HALT = 45,
	ACTION_UNKNOWN
} SMPL_ACTION;

struct simpletron* init_simpletron(const int pages_size, const int words_size, const int word_size);
void tron_free(struct simpletron* self);
void tron_move(struct simpletron* self, unsigned char* dest, const unsigned char* src);
void tron_dump(struct simpletron* self);
void tron_dump_page(struct simpletron* self, int n);
void tron_run(struct simpletron* self, const char* filename);
void tron_print_mem_loc(struct simpletron* self, unsigned char* val);

void tron_write(struct simpletron* self);
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
void tron_divide(struct simpletron* self);
void tron_dividex(struct simpletron* self);
void tron_multiply(struct simpletron* self);
void tron_multiplyx(struct simpletron* self);
void tron_inc(struct simpletron* self);
void tron_dec(struct simpletron* self);
void tron_swap(struct simpletron* self);
void tron_halt(struct simpletron* self);
void tron_branch(struct simpletron* self);
void tron_branchneg(struct simpletron* self);
void tron_branchzero(struct simpletron* self);
#endif
