const int VERSION = 9;
const char PASSWORD[4] = "SVU";

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
    IN              = 19,
    START           = 20,
    INVALID_COMMAND = 21,
    HLT             = 22
} command_t;

typedef enum
{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3,
    SX = 4
} reg_t;
