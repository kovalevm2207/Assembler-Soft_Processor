const int VERSION = 6;
const char PASSWORD[] = "SPbSVU";

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
    START           = 12,
    INVALID_COMMAND = 13,
    HLT             = 14
} command_t;

typedef enum
{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3
} reg_t;
