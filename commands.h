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

//todo(сам себе): Пустые строки сбивают нумерацию в данной реализации
                // Внутри структуры line добавить ячейку с номер строчки
                // очень полезно для ...Dump (можно будет даже ctrl + click делать)

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

const int PRECISION = 100;
const size_t CAPACITY = 20;
const int COMMANDS_NUM = 26;
const int REGS_NUM = 6;
const size_t START_PC = 2;

const int DELAY_MS = 33;
const int OX_SIZE = 165 * 2;            // для кружочка 111
const int RAM_SIZE = 165 * 92 * 2;      //define KoV -- must have (mentor, please, help him)
                                        // mentor said that it's a base and it's OK ))))) Мика

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
    DX = 3,                                     //struct reg = { {const char* name}, int reg_number}, {},}; KoV
    EX = 4,                                     // на случай если регистры будут уже не типовыми, пока держимся так
    FX = 5                                      // так что рассматриваем этот коммент пока как идею для реализации на будущее
} reg_t;


typedef enum
{
    CMD_OK      = 1 << 1,
    INVALID_CMD = 1 << 2,
    TOTAL_ERR   = 1 << 3,
    CMD_TRUE    = 1 << 4,
    CMD_FALSE   = 1 << 5,
    NFIND_CMD   = 1 << 6,
    NFIND_ARG   = 1 << 7,
    NFIND_REG   = 1 << 8,
    INVALID_REG = 1 << 9,
    NFIND_LBL   = 1 << 10
} cmdErr_t;


typedef struct translator_s
{
    size_t linenum;
    size_t count_line;
    size_t code_num;                   //size_t KoV  не помню вообще чего тут((( Мика
    line* lines;
    int* codes;
    int labels[MAX_LABELS_NUM];    //size_t KoV  не мне конечно память не жалко, но вряд ли у меня метка будет тяжелее чем int Мика
} translator_s;


typedef struct SPU
{
    int* code;
    size_t PC;
    stack_s stk;
    int regs[REGS_NUM];       //define REG_NUMBER не const int KoV   ну нее, ну не, какой DEFINE ----> const int, а вот просто цифру уберу Мика
    size_t file_size;
    stack_s labels;
    int* RAM;
} SPU;


typedef struct command_s{
    cmdErr_t (*func_exe)(SPU* spu, const command_s* command);
    char name[MAX_COMMAND_LENGTH];                              //define 16  KoV // define --> NO , а вот имя вместо числа вставлю Мика
    command_t num;                                               // enum а не int KoV
    long int hash;
    cmdErr_t (*func_asm)(translator_s* translator);
} command_s;


//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//
cmdErr_t nothing_asm(translator_s* translator);
cmdErr_t arg_asm(translator_s* translator);
cmdErr_t reg_asm(translator_s* translator);
cmdErr_t lbl_asm(translator_s* translator);
//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//

//===============================================================
//===============================================================

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//

//-----------------------------------------------------------PUSH
cmdErr_t push_exe(SPU* spu, const command_s* commands);
cmdErr_t pushreg_exe(SPU* spu, const command_s* command);
cmdErr_t pushm_exe(SPU* spu, const command_s* command);
//-----------------------------------------------------------PUSH
//-----------------------------------------------------------POP
cmdErr_t popreg_exe(SPU* spu, const command_s* command);
cmdErr_t popm_exe(SPU* spu, const command_s* command);
//-----------------------------------------------------------POP
//-----------------------------------------------------------MATH
cmdErr_t math_exe(SPU* spu, const command_s* command);
cmdErr_t pow_exe(SPU* spu, const command_s* command);
cmdErr_t my_sqvrt_exe(SPU* spu, const command_s* command);
//-----------------------------------------------------------MATH
//-----------------------------------------------------------JMPs
cmdErr_t jmp_exe(SPU* spu, const command_s* command);
cmdErr_t call_exe(SPU* spu, const command_s* command);
cmdErr_t ret_exe(SPU* spu, const command_s* command);
//------------------------------------------------------------JMPs
//------------------------------------------------------------SYSTEM
cmdErr_t out_exe(SPU* spu, const command_s* command);
cmdErr_t reset_stk_exe(SPU* spu, const command_s* command);
cmdErr_t in_exe(SPU* spu, const command_s* command);
cmdErr_t draw_exe(SPU* spu, const command_s* command);
cmdErr_t hlt_exe(SPU* spu, const command_s* command);
cmdErr_t help_exe(SPU* spu, const command_s* command);
//------------------------------------------------------------SYSTEM

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//
const command_s commands[COMMANDS_NUM] =
{
 {push_exe,      "PUSH",      PUSH,      2467610,        arg_asm},
 {math_exe,      "ADD",       ADD,       64641,          nothing_asm},
 {math_exe,      "MOD",       MOD,       76514,          nothing_asm},
 {math_exe,      "SUB",       SUB,       82464,          nothing_asm},
 {math_exe,      "DIV",       DIV,       67697,          nothing_asm},
 {out_exe,       "OUT",       OUT,       78638,          nothing_asm},
 {math_exe,      "MUL",       MUL,       76708,          nothing_asm},
 {pow_exe,       "POW",       POW,       79416,          arg_asm},
 {my_sqvrt_exe,  "SQRT",      SQRT,      2553120,        nothing_asm},     //sqrt -> sqvrt KoV сделал конечно))))) Мика
 {reset_stk_exe, "RESET_STK", RESET_STK, 71911154149114, nothing_asm},
 {pushreg_exe,   "PUSHREG",   PUSHREG,   73512650522,    reg_asm},
 {popreg_exe,    "POPREG",    POPREG,    2365754531,     reg_asm},
 {jmp_exe,       "JB",        JB,        2360,           lbl_asm},       //одна функция для exe и второй столбик уберётся KoV
 {jmp_exe,       "JBE",       JBE,       73229,          lbl_asm},        // (сделал) Мика
 {jmp_exe,       "JA",        JA,        2359,           lbl_asm},
 {jmp_exe,       "JAE",       JAE,       73198,          lbl_asm},
 {jmp_exe,       "JE",        JE,        2363,           lbl_asm},
 {jmp_exe,       "JNE",       JNE,       73601,          lbl_asm},
 {call_exe,      "CALL",      CALL,      2060894,        lbl_asm},     // todo (от ментора): maybe отдельный массив с hash,
 {ret_exe,       "RET",       RET,       81025,          nothing_asm}, // чтобы вручную не заполнять
 {in_exe,        "IN",        IN,        2341,           nothing_asm},
 {pushm_exe,     "PUSHM",     PUSHM,     76495987,       reg_asm},
 {popm_exe,      "POPM",      POPM,      2461756,        reg_asm},
 {draw_exe,      "DRAW",      DRAW,      2106692,        nothing_asm},
 {hlt_exe,       "HLT",       HLT,       71632,          nothing_asm},
 {help_exe,      "HELP",      HELP,      2213697,        nothing_asm}
};

#endif // COMMANDS
