#ifndef COMMANDS
#define COMMANDS

const int VERSION = 12;
const char SIGNATURE[4] = "SVU";

typedef enum
{
    HELP            = 0,
    PUSH            = 1,      //=============================================//
    ADD             = 2,      //                                             //
    MOD             = 3,      //                                             //
    SUB             = 4,      //         IF YOU CHANGE THE COMMANDS.H        //
    DIV             = 5,      //                                             //
    OUT             = 6,      //                 YOU MUST                    //
    MUL             = 7,      //                                             //
    POW             = 8,      //            CHANGE THE VERSION               //
    SQRT            = 9,      //                                             //
    RESET_STK       = 10,     //=============================================//
    PUSHREG         = 11,
    POPREG          = 12,
    JMP             = 13,
    JB              = 14, // <
    JBE             = 15, // <=
    JA              = 16, // >
    JAE             = 17, // >=
    JE              = 18, // ==
    JNE             = 19, // !=
    CALL            = 20,
    RET             = 21,
    IN              = 22,
    PUSHM           = 23,
    POPM            = 24,
    DRAW            = 25,
    START           = 26,
    INVALID_COMMAND = 27,
    HLT             = 28
} command_t;

typedef enum
{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3,
    SX = 4,
    RX = 5
} reg_t;

#endif // COMMANDS
