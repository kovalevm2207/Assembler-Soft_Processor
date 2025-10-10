IN                                                        # коэффициент a
IN                                 # Ввод значений:       # коэффициент b
IN                                                        # коэффициент c
POPREG CX
POPREG BX                          # Присвоение регистрам значений коэффициентов
POPREG AX
PUSH 0                             # Сравнение AX с нулем
PUSHREG AX
JE 105                             # AX =? 0
    PUSHREG BX                    # Случай a != 0, b - любое, 0 c - любое
    POW 2                         # Считаем дискриминант
    PUSH 4
    PUSHREG AX
    PUSHREG CX
    MUL
    MUL
    SUB
    POPREG DX
    PUSHREG DX                    # Сравниваем DX
    PUSH 0
    JA 66                         # DX >? 0
        PUSHREG DX
        PUSH 0
        JB 60                           # DX <? 0
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
        PUSH 0                      # Случай DX < 0
        OUT                         # Выводим количество корней (0)
        HLT
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
PUSH -1                 # Случай a == 0
OUT                     # Выводим количество корней (-1)
HLT
