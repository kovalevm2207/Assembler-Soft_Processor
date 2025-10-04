const int VERSION = 2;
const char PASSWORD[] = "SPbSVU";

typedef enum
{
    HELP            = 0,
    PUSH            = 1,      //=============================================//
    ADD             = 2,      //                                             //
    SUB             = 3,      //         IF YOU CHANGE THE COMMANDS          //
    DIV             = 4,      //                                             //
    OUT             = 5,      //                 YOU MUST                    //
    MUL             = 6,      //                                             //
    POW             = 7,      //            CHANGE THE VERSION               //
    SQRT            = 8,      //                                             //
    RESET_STK       = 9,      //=============================================//
    START           = 10,
    INVALID_COMMAND = 11,
    HLT             = 12
} command_t;
