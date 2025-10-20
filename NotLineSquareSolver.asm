#программа для решения квадратного уравнения
IN                                                        # коэффициент a
IN                                 # Ввод значений:       # коэффициент b
IN                                                        # коэффициент c
POPREG CX
POPREG BX                          # Присвоение регистрам значений коэффициентов
POPREG AX
PUSH 0                             # Сравнение AX с нулем
PUSHREG AX
JE :1                              # AX =? 0
    CALL :4
    CALL :4
    PUSHREG DX          # Случай a != 0, b - любое, 0 c - любое
    PUSH 0              # Сравниваем DX
    JA :2                         # DX >? 0
        PUSHREG DX
        PUSH 0
        JB :3                           # DX <? 0
            PUSH 1                      # Случай DX == 0
            OUT                         # Выводим количество корней (1)
            PUSHREG BX
            PUSHREG AX
            DIV
            PUSH 2
            DIV
            PUSH -1
            MUL
            OUT                         # Первый корень
            HLT
        :3
        PUSH 0                      # Случай DX < 0
        OUT                         # Выводим количество корней (0)
        HLT
    :2
    PUSHREG DX                  # Случай DX > 0
    PUSH 2
    OUT                         # Выводим количество корней (2)
    PUSHREG DX
    SQRT
    POPREG DX
    PUSHREG BX
    PUSH -1
    MUL
    PUSHREG DX
    ADD
    PUSH 2
    PUSHREG AX
    MUL
    DIV
    OUT                             # Первый корень
    PUSHREG BX
    PUSH -1
    MUL
    PUSHREG DX
    SUB
    PUSH 2
    PUSHREG AX
    MUL
    DIV
    OUT                              # Второй корень
    HLT
:1
PUSH -1                 # Случай a == 0
OUT                     # Выводим количество корней (-1)
HLT
:4                      # Считаем дискриминант
PUSHREG BX
POW 2
PUSH 4
PUSHREG AX
PUSHREG CX
MUL
MUL
SUB
POPREG DX
RET
