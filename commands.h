//=============================================//
//                                             //
//                                             //
//         IF YOU CHANGE THE COMMANDS.H        //
//                                             //
//                 YOU MUST                    //
//                                             //
//            CHANGE THE VERSION               //
//                                             //
//=============================================//

// Пустые строки сбивают нумерацию в данной реализации

#ifndef COMMANDS
#define COMMANDS

#include <stdio.h>
#include <math.h>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>

#include "my_stack.h"
#include "ReadFile.h"

typedef struct heder
{
    static const int VERSION = 13;
    static constexpr char SIGNATURE[4] = "SVU";
}heder;

const int MAX_COMMAND_LENGTH = 10;
const int MAX_LABELS_NUM = 20;
const int MAX_REG_LENGTH = 8;

const size_t CAPACITY = 20;
const int COMMANDS_NUM = 26;
const int REGS_NUM = 6;

const int DELAY_MS = 100;
const int OX_SIZE = 165 * 2;  // для кружочка 111
const int RAM_SIZE = 165 * 92 * 2;

typedef enum
{
    PUSH            = 0,
    ADD             = 1,
    MOD             = 2,
    SUB             = 3,
    DIV             = 4,
    OUT             = 5,
    MUL             = 6,
    POW             = 7,
    SQRT            = 8,
    RESET_STK       = 9,
    PUSHREG         = 10,
    POPREG          = 11,
    JB              = 12, // <
    JBE             = 13, // <=
    JA              = 14, // >
    JAE             = 15, // >=
    JE              = 16, // ==
    JNE             = 17, // !=
    CALL            = 18,
    RET             = 19,
    IN              = 20,
    PUSHM           = 21,
    POPM            = 22,
    DRAW            = 23,
    HLT             = 24,
    HELP            = 25
} command_t;


typedef enum
{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3,
    EX = 4,
    FX = 5
} reg_t;


typedef struct translator_s
{
    size_t linenum;
    size_t count_line;
    int code_num;
    line* lines;
    int* codes;
    int labels[MAX_LABELS_NUM];
} translator_s;


typedef struct SPU
{
    int* code;
    size_t PC;
    stack_s stk;
    int regs[5];
    size_t file_size;
    stack_s labels;
    int* RAM;
} SPU;


typedef struct command_s{
    void (*func_exe)(SPU* spu, const command_s* command);
    int (*action_exe)(int a, int b);
    char name[16];
    int num;
    long int hash;
    int (*func_asm)(translator_s* translator);
} command_s;


//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//
int nothing_asm(translator_s* translator);
int arg_asm(translator_s* translator);
int reg_asm(translator_s* translator);
int lbl_asm(translator_s* translator);
//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//

//===============================================================
//===============================================================

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//

//-----------------------------------------------------------PUSH
int nothing_exe(int a, int b);
void push_exe(SPU* spu, const command_s* commands);
void pushreg_exe(SPU* spu, const command_s* command);
void pushm_exe(SPU* spu, const command_s* command);
//-----------------------------------------------------------PUSH
//-----------------------------------------------------------POP
void popreg_exe(SPU* spu, const command_s* command);
void popm_exe(SPU* spu, const command_s* command);
//-----------------------------------------------------------POP
//-----------------------------------------------------------MATH
int add_exe(int a, int b);
int sub_exe(int a, int b);
int mod_exe(int a, int b);
int my_div_exe(int a, int b);
int mul_exe(int a, int b);
void math_exe(SPU* spu, const command_s* command);
void pow_exe(SPU* spu, const command_s* command);
void my_sqrt_exe(SPU* spu, const command_s* command);
//-----------------------------------------------------------MATH
//-----------------------------------------------------------JMPs
int jb_exe(int a, int b);
int jbe_exe(int a, int b);
int ja_exe(int a, int b);
int jae_exe(int a, int b);
int je_exe(int a, int b);
int jne_exe(int a, int b);
void jmp_exe(SPU* spu, const command_s* command);
void call_exe(SPU* spu, const command_s* command);
void ret_exe(SPU* spu, const command_s* command);
//------------------------------------------------------------JMPs
//------------------------------------------------------------SYSTEM
void out_exe(SPU* spu, const command_s* command);
void reset_stk_exe(SPU* spu, const command_s* command);
void in_exe(SPU* spu, const command_s* command);
void draw_exe(SPU* spu, const command_s* command);
void hlt_exe(SPU* spu, const command_s* command);
void help_exe(SPU* spu, const command_s* command);
//------------------------------------------------------------SYSTEM

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//
const command_s commands[COMMANDS_NUM] =
{
 {push_exe,      nothing_exe, "PUSH",      PUSH,      2467610,        arg_asm},
 {math_exe,      add_exe,     "ADD",       ADD,       64641,          nothing_asm},
 {math_exe,      mod_exe,     "MOD",       MOD,       76514,          nothing_asm},
 {math_exe,      sub_exe,     "SUB",       SUB,       82464,          nothing_asm},
 {math_exe,      my_div_exe,  "DIV",       DIV,       67697,          nothing_asm},
 {out_exe,       nothing_exe, "OUT",       OUT,       78638,          nothing_asm},
 {math_exe,      mul_exe,     "MUL",       MUL,       76708,          nothing_asm},
 {pow_exe,       nothing_exe, "POW",       POW,       79416,          arg_asm},
 {my_sqrt_exe,   nothing_exe, "SQRT",      SQRT,      2553120,        nothing_asm},
 {reset_stk_exe, nothing_exe, "RESET_STK", RESET_STK, 71911154149114, nothing_asm},
 {pushreg_exe,   nothing_exe, "PUSHREG",   PUSHREG,   73512650522,    reg_asm},
 {popreg_exe,    nothing_exe, "POPREG",    POPREG,    2365754531,     reg_asm},
 {jmp_exe,       jb_exe,      "JB",        JB,        2360,           lbl_asm},
 {jmp_exe,       jbe_exe,      "JBE",       JBE,      73229,          lbl_asm},
 {jmp_exe,       ja_exe,      "JA",        JA,        2359,           lbl_asm},
 {jmp_exe,       jae_exe,     "JAE",       JAE,       73198,          lbl_asm},
 {jmp_exe,       je_exe,      "JE",        JE,        2363,           lbl_asm},
 {jmp_exe,       jne_exe,     "JNE",       JNE,       73601,          lbl_asm},
 {call_exe,      nothing_exe, "CALL",      CALL,      2060894,        lbl_asm},
 {ret_exe,       nothing_exe, "RET",       RET,       81025,          nothing_asm},
 {in_exe,        nothing_exe, "IN",        IN,        2341,           nothing_asm},
 {pushm_exe,     nothing_exe, "PUSHM",     PUSHM,     76495987,       reg_asm},
 {popm_exe,      nothing_exe, "POPM",      POPM,      2461756,        reg_asm},
 {draw_exe,      nothing_exe, "DRAW",      DRAW,      2106692,        nothing_asm},
 {hlt_exe,       nothing_exe, "HLT",       HLT,       71632,          nothing_asm},
 {help_exe,      nothing_exe, "HELP",      HELP,      2213697,        nothing_asm}
};

#endif // COMMANDS
