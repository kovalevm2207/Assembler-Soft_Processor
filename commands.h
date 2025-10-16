#ifndef COMMANDS
#define COMMANDS

const int VERSION = 10;
const char SIGNATURE[4] = "SVU";

typedef enum
{
    HELP            = 0,
    PUSH            = 1,      //=============================================//
    ADD             = 2,      //                                             //
    SUB             = 3,      //         IF YOU CHANGE THE COMMANDS.H        //
    DIV             = 4,      //                                             //
    OUT             = 5,      //                 YOU MUST                    //
    MUL             = 6,      //                                             //
    POW             = 7,      //            CHANGE THE VERSION               //
    SQRT            = 8,      //                                             //
    RESET_STK       = 9,      //=============================================//
    PUSHREG         = 10,
    POPREG          = 11,
    JMP             = 12,
    JB              = 13, // <
    JBE             = 14, // <=
    JA              = 15, // >
    JAE             = 16, // >=
    JE              = 17, // ==
    JNE             = 18, // !=
    CALL            = 19,
    RET             = 20,
    IN              = 21,
    START           = 22,
    INVALID_COMMAND = 23,
    HLT             = 24
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
