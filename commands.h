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

const size_t CAPACITY = 20;
const int COMMANDS_NUM = 25;
const int REGS_NUM = 6;
const int MAX_LABELS_NUM = 20;
const int MAX_REG_LENGTH = 8;
const int RAM_SIZE = 9213;

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
    START           = 25,
    INVALID_COMMAND = 26,
    HELP            = 27
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


struct translator_s
{
    size_t linenum;
    size_t count_line;
    int code_num;
    line* lines;
    int* codes;
    int labels[MAX_LABELS_NUM];
};


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
    char name[16];
    int num;
    int (*func_asm)(translator_s* translator);
    int (*action_exe)(int a, int b);
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
//------------------------------------------------------------SYSTEM

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//
const command_s commands[COMMANDS_NUM] =
{
 {push_exe,      "PUSH",      PUSH,      arg_asm,     nothing_exe},
 {math_exe,      "ADD",       ADD,       nothing_asm, add_exe},
 {math_exe,      "MOD",       MOD,       nothing_asm, mod_exe},
 {math_exe,      "SUB",       SUB,       nothing_asm, sub_exe},
 {math_exe,      "DIV",       DIV,       nothing_asm, my_div_exe},
 {out_exe,       "OUT",       OUT,       nothing_asm, nothing_exe},
 {math_exe,      "MUL",       MUL,       nothing_asm, mul_exe},
 {pow_exe,       "POW",       POW,       arg_asm,     nothing_exe},
 {my_sqrt_exe,   "SQRT",      SQRT,      nothing_asm, nothing_exe},
 {reset_stk_exe, "RESET_STK", RESET_STK, nothing_asm, nothing_exe},
 {pushreg_exe,   "PUSHREG",   PUSHREG,   reg_asm,     nothing_exe},
 {popreg_exe,    "POPREG",    POPREG,    reg_asm,     nothing_exe},
 {jmp_exe,       "JB",        JB,        lbl_asm,     jb_exe},
 {jmp_exe,       "JBE",       JBE,       lbl_asm,     jb_exe},
 {jmp_exe,       "JA",        JA,        lbl_asm,     ja_exe},
 {jmp_exe,       "JAE",       JAE,       lbl_asm,     jae_exe},
 {jmp_exe,       "JE",        JE,        lbl_asm,     je_exe},
 {jmp_exe,       "JNE",       JNE,       lbl_asm,     jne_exe},
 {call_exe,      "CALL",      CALL,      lbl_asm,     nothing_exe},
 {ret_exe,       "RET",       RET,       nothing_asm, nothing_exe},
 {in_exe,        "IN",        IN,        nothing_asm, nothing_exe},
 {pushm_exe,     "PUSHM",     PUSHM,     reg_asm,     nothing_exe},
 {popm_exe,      "POPM",      POPM,      reg_asm,     nothing_exe},
 {draw_exe,      "DRAW",      DRAW,      nothing_asm, nothing_exe},
 {hlt_exe,       "HLT",       HLT,       nothing_asm, nothing_exe},
};

#endif // COMMANDS
