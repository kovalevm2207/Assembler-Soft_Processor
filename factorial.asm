IN
POPREG AX
CALL :1
OUT
HLT
:1
PUSHREG AX
PUSH 0
JE :2
    PUSHREG AX
    PUSH 1
    JE :2
        PUSHREG AX
        PUSHREG AX
        PUSH 1
        SUB
        POPREG AX
        CALL :1
        MUL
        RET
:2
PUSH 1
RET
