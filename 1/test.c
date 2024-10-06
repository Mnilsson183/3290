#include "assert.h"
#include "tron.h"
#include <stdlib.h>
#include <stdio.h>

#define TEST_PAGES_SIZE 2
#define TEST_WORDS_SIZE 100
#define TEST_WORD_SIZE 7

void test_tron_move(void) {
    printTestingSegment("Tron Move");
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);

    unsigned char buf[] = "+000001";

    tron_move(tron, tron->index_reg, buf);
    assertStringNEqual((char*)buf, (char*)tron->index_reg, tron->word_size, "Setting a value into the index register");

    tron_free(tron);
    allTestsPassing("Move");
}

void test_tron_move_n(void) {
    printTestingSegment("Tron Move N");
    //struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);

    allTestsPassing("Tron Move N");
}

void test_tron_resize_value() {
    printTestingSegment("Resizing values");
    unsigned char* src = (unsigned char*)"+234";
    unsigned char dest[7];
    tron_resize_value(dest, src, 4, 7);
    assertStringNEqual((const char*)dest, "+000234", 7,"Resizing a value that has a sign");

    src = (unsigned char*)"-102";
    tron_resize_value(dest, src, 4, 7);
    assertStringNEqual((const char*)dest, "-000102", 7,"Resizing a value that has a sign");

    allTestsPassing("Resize values");
}

void test_tron_resize_unsigned_to_signed(void) {
    printTestingSegment("Resize unsigned to signed");

    unsigned char* src = (unsigned char*)"234";
    unsigned char dest[7];
    tron_resize_unsigned_to_signed(dest, src, 3, 7);
    assertStringNEqual((const char*)dest, "+000234", 7,"Resizing a value that has no sign");

    allTestsPassing("Resize unsigned to signed");
}

void test_parse_instruct(void) {
    printTestingSegment("Parse Instructions");
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    parse_instruct(tron, (unsigned char*)"101221");
    assertStringNEqual((char *)tron->opCode, "10", 2, "Parsing Instruction can parse the opcode");
    assertStringNEqual((char *)tron->operand, "1221", 4, "Parsing Instruction can parse the operand");

    tron_free(tron);
    allTestsPassing("Parse Instructions");
}

void test_char_to_int_n(void) {
    printTestingSegment("Chars To Int");
    int val = char_to_int_n((unsigned char *)"+123456", 7);
    assertEquals(123456, val, "Convert positive chars to an Int");

    val = char_to_int_n((unsigned char *)"-123456", 7);
    assertEquals(-123456, val, "Convert negative chars to an Int");

    val = char_to_int_n((unsigned char *)"+003456", 7);
    assertEquals(3456, val, "Confrom_vert positive chars to an Int with leading zeros");

    val = char_to_int_n((unsigned char *)"-003456", 7);
    assertEquals(-3456, val, "Convert negative chars to an Int with leading zeros");

    allTestsPassing("Chars To Int");
}

void test_tron_get_from_reg(void) {
    printTestingSegment("Get From memory from Register");
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    // load an adress into the operand
    // page 1 word 3
    unsigned char* loc = tron_get_from_reg(tron, (unsigned char *)"0103");
    unsigned char* correct = &tron->memory[1][3 * tron->word_size];
    assertPtrEquals(loc, correct, "You can get the memory location of word");


    tron_free(tron);
    allTestsPassing("Get Register");
}


//int READ=10 Read a word from the terminal into a location whose address is the operand
void test_tron_read(void) {
}
//int WRITE=11 Write a word from the location whose address is the operand to the terminal
//void tron_write(struct simpletron* self) {
//
//}
//int LOAD=20 Load a word from the memory location specified by the operand into the accumlator
void test_tron_load(void) {
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    unsigned char* word = (unsigned char*)"0102";
    // load loc into operand
    tron_move_n(tron->operand, word, 4);
    tron_load(tron);
    unsigned char* mem = (unsigned char*)&tron->memory[1][2 * tron->word_size];
    assertStringNEqual(tron->accum, mem, 7, "You can use the load instruction");
    tron_free(tron);
}
//int LOADIM=21 Load the operand into the accumulator
void test_tron_loadim() {
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    unsigned char* word = (unsigned char*)"0102";
    // load loc into operand
    tron_move_n(tron->operand, word, 4);
    tron_loadim(tron);
    assertStringNEqual(tron->accum, "+000102", 7, "LOADIM instruction");
    tron_free(tron);
}
//int LOADX=22 Load word from the memory location specified by the operand into the index register
void test_tron_loadx() {
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    unsigned char* word = (unsigned char*)"0102";
    // load loc into operand
    tron_move_n(tron->operand, word, 4);
    tron_loadim(tron);
    unsigned char* mem = (unsigned char*)&tron->memory[1][2 * tron->word_size];
    assertStringNEqual(tron->index_reg, mem, 7, "You can use the load instruction");
    tron_free(tron);
}
//int LOADIDX=22 Load word from the memory location specified by index register into accumulator
void test_tron_loadidx() {
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    unsigned char* word = (unsigned char*)"+000102";
    tron_move_n(tron->index_reg, word, 7);
    tron_loadidx(tron);
    unsigned char* mem = (unsigned char*)&tron->memory[1][2 * tron->word_size];
    assertStringNEqual(tron->accum, mem, 7, "You can use the loadidx instruction");
    tron_free(tron);
}
//int STORE=25 Store a word from the acccumulator into the memory location specified by the operand
void test_tron_store() {
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    unsigned char* loc = (unsigned char*)"0102";
    unsigned char* word = (unsigned char*)"-123456";
    // load loc into operand
    tron_move_n(tron->operand, loc, 4);
    tron_move(tron, tron->accum, word);
    tron_store(tron);
    unsigned char* mem = (unsigned char*)&tron->memory[1][2 * tron->word_size];
    assertStringNEqual(mem, word, tron->word_size, "You can use the store instruction");
}
//int STOREIDX=26 store a word from the accumulator into a memory location specified by index register
void test_tron_storeidx() {
    struct simpletron* tron = init_simpletron(TEST_PAGES_SIZE, TEST_WORDS_SIZE, TEST_WORD_SIZE);
    unsigned char* loc = (unsigned char*)"+000102";
    unsigned char* word = (unsigned char*)"-123456";
    // load loc into operand
    tron_move(tron, tron->index_reg, loc);
    tron_move(tron, tron->accum, word);
    tron_storeidx(tron);
    unsigned char* mem = (unsigned char*)&tron->memory[1][2 * tron->word_size];
    assertStringNEqual(mem, word, tron->word_size, "You can use the storeidx instruction");
}
//int ADD=30 Add the word in memory whose address is the operand to the accumulator and leave result in accumulator ( ACC += MEM )
void test_tron_add() {
}
//int ADDX=31 Add a word in memory whose address is stored in index register to the accumulator and leave result in accumulator
void test_tron_addx() {

}
//int SUBTRACT=32 Subtract a word whose address stored in the operand from the accumulator and leave result in accumulator ( ACC -= MEM )
void test_tron_subtract() {

}
//int SUBTRACTX=33 Subtract a word whose address is stored in the index register from the accumulator and leave result in accumulator
void test_tron_subtractx() {

}
//int DIVIDE=34 Divide the accumulator by a word whose address stored in the operand and leave result in accumulator and lose the remainder.( ACC /= MEM )
void tron_tron_divide() {

}
//int DIVIDEX=35 Divide the accumulator by a word whose address is stored in the index register and leave result in accumulator and lose the remainder.
void test_tron_dividex() {

}
//int MULTIPLY=36 Multiply the accumulator by a word from a specific location in memory and leave result in accumulator ( ACC *= MEM )
void test_tron_multiply() {

}
//int MULTIPLYX=37 Multiply the accumulator by a word whose address is stored in the index register and leave result in accumulator
void test_tron_multiplyx() {

}
//int INC=38 increase index register by 1
void test_tron_inc() {

}
//int DEC=39 decrease index register by 1
void test_tron_dec() {

}
//int BRANCH=40 Branch to a specific location in memory, location address is in operand
void test_tron_branch() {

}
//int BRANCHNEG=41 Branch to a specific location in memory if accumulator is negative
void test_tron_branchneg() {

}
//int BRANCHZERO=42 Branch to a specific location in memory if the accumulator is zero
void test_tron_branchzero() {

}
//int SWAP=43 swap contents of index register and accumulator
void test_tron_swap() {

}
//int HALT=45 halt program dump register values and a range of pages. The starting page of the range is stored as the top 2 digits of the operand and the last page as the least significant 2 digits( core dump ).
void test_tron_halt() {

}

void test_tron_instructions(void) {
    printTestingSegment("Instructions");
    test_tron_load();
    test_tron_loadim();
    test_tron_loadx();
    test_tron_loadidx();
    test_tron_store();
    test_tron_storeidx();
    //test_tron_add();
    //test_tron_addx();
    //test_tron_subtract();
    //test_tron_subtractx();
    //tron_tron_divide();
    //test_tron_dividex();
    //test_tron_multiply();
    //test_tron_multiplyx();
    //test_tron_inc();
    //test_tron_dec();
    //test_tron_branch();
    //test_tron_branchneg();
    //test_tron_branchzero();
    //test_tron_swap();
    //test_tron_halt();
    allTestsPassing("Insructions");
}

int main(void) {
    test_tron_move();
    test_tron_resize_value();
    test_tron_resize_unsigned_to_signed();
    test_parse_instruct();
    test_char_to_int_n();
    test_tron_get_from_reg();
    test_tron_instructions();
    allTestsPassing("Project");
}
