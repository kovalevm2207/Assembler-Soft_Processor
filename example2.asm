# Вывод квадратов каких-то там чисел
IN
POPREG AX
IN
POPREG BX
:7
    PUSHREG AX
    PUSHREG AX
    MUL
    OUT
    PUSHREG AX
    PUSH 1
    ADD
    POPREG AX
    PUSHREG AX
    PUSHREG BX
    JBE :7
HLT
