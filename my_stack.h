#ifndef MY_STACK
#define MY_STACK

#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#include "color_print.h"

typedef int stack_t;

const int MAXSIZE = 1'000'000'000; // or todo 1e9
const size_t L_CANARY = 0xBADDED;
const size_t R_CANARY = 0xDEDBAD;
const size_t POISON = 0xABCCBA;

typedef enum
{
    ALL_OK              = 0 << 0,
    BAD_DATA_PTR        = 1 << 1,
    BAD_SIZE_L          = 1 << 2,
    BAD_SIZE_R          = 1 << 3,
    BAD_START_CAPACITY  = 1 << 4,
    BAD_STK_CAPACITY    = 1 << 5,
    BAD_STK_PTR         = 1 << 6,
    BAD_VALUE           = 1 << 7,
    LEFT_CANARY_DEAD    = 1 << 8,
    RIGHT_CANARY_DEAD   = 1 << 9
} StackErr_t;

struct stack_s
{
    stack_t* data;
    size_t size;
    size_t capacity;
};

StackErr_t StackCtor_(stack_s* stk, size_t capacity, const char* file, int line);
StackErr_t StackPush_(stack_s* stk, stack_t value, const char* file, int line);
StackErr_t StackPop_(stack_s* stk, stack_t* value, const char* file, int line);
StackErr_t StackDtor_(stack_s* stk, const char* file, int line);

#define StackCtor(stk, capacity) StackCtor_(stk, capacity, __FILE__, __LINE__)
#define StackPush(stk, value) StackPush_(stk, value, __FILE__, __LINE__)
#define StackPop(stk, value) StackPop_(stk, value, __FILE__, __LINE__)
#define StackDtor(stk) StackDtor_(stk, __FILE__, __LINE__)

int StackVerify(stack_s* stk, int STATUS); // todo rename (add Stack)
void StackDump(stack_s* stk, int STATUS);
void do_RCanaryErr(stack_s* stk);
void do_LCanaryErr(stack_s* stk);
void print_data(stack_s* stk, int STATUS);


#define START_OK if (capacity > MAXSIZE) STATUS |= BAD_START_CAPACITY;                              \
    if (stk == NULL) STATUS |= BAD_STK_PTR;                                                         \
    if (STATUS != ALL_OK) {                                                                         \
        printf("\n" CHANGE_ON BOLD WITH LIGHT_RED TEXT_COLOR "StackDump() from %s at %s:%d\n" RESET,\
        __func__, file, line);                                                                      \
        StackDump(stk, STATUS);                                                                     \
        return (StackErr_t) STATUS;                                                                 \
    }

#define STACK_OK STATUS = StackVerify(stk, STATUS);                                                 \
    if (STATUS != ALL_OK) {                                                                         \
        printf("\n" CHANGE_ON BOLD WITH LIGHT_RED TEXT_COLOR "StackDump() from %s at %s:%d\n" RESET,\
        __func__, file, line);                                                                      \
        StackDump(stk, STATUS);                                                                     \
        return (StackErr_t) STATUS;                                                                 \
    }

StackErr_t StackCtor_(stack_s* stk, size_t capacity, const char* file, int line)
{
    int STATUS = ALL_OK;
    START_OK // todo better make new function

    stk->data = (stack_t*) calloc(capacity  + 2, sizeof(stack_t));
    stk->size = 1;
    stk->capacity = capacity;
    stk->data[0] = L_CANARY;
    stk->data[capacity + 1] = R_CANARY;
    for (size_t i = 1; i < capacity + 1; i++) stk->data[i] = POISON;
    STACK_OK
    return ALL_OK;
}

StackErr_t StackPush_(stack_s* stk, stack_t value, const char* file, int line)
{
    int STATUS = ALL_OK;
    if (value > MAXSIZE) STATUS |= BAD_VALUE;
    STACK_OK

    stk->data[stk->size] = value;
    stk->size++;

    STACK_OK

    return ALL_OK;
}

StackErr_t StackPop_(stack_s* stk, stack_t* value, const char* file, int line)
{
    int STATUS = ALL_OK;
    if (value == NULL) STATUS |= BAD_VALUE;
    STACK_OK

    stk->size--;
    *value = stk->data[stk->size];
    stk->data[stk->size] = POISON;

    STACK_OK

    return ALL_OK;
}

StackErr_t StackDtor_(stack_s* stk, const char* file, int line)
{
    int STATUS = ALL_OK;
    STACK_OK

    free(stk->data);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;
    stk = NULL;

    STACK_OK

    return ALL_OK;
}

int StackVerify(stack_s* stk, int STATUS) // todo add poisons
{
    if (stk == NULL) {
       STATUS |= BAD_STK_PTR;
       return STATUS;
    }
    if (stk->capacity > MAXSIZE)                  STATUS |= BAD_STK_CAPACITY;
    if (stk->size > stk->capacity + 1)            STATUS |= BAD_SIZE_R;
    if (stk->size < 1)                            STATUS |= BAD_SIZE_L;
    if (stk->data == NULL)                        STATUS |= BAD_DATA_PTR;
    else {
        if (stk->data[0] != L_CANARY)                 STATUS |= LEFT_CANARY_DEAD;
        if (stk->data[stk->capacity + 1] != R_CANARY) STATUS |= RIGHT_CANARY_DEAD;
    }
    return STATUS;
}

void StackDump(stack_s* stk, int STATUS)
{
    if (stk == NULL) {
        STATUS |= BAD_STK_PTR;
        return;
    }
    if (STATUS & ALL_OK) {
        printf(CHANGE_ON BOLD WITH GREEN TEXT_COLOR "ALL_OK(%d) ", ALL_OK);
        return;
    }

    printf("stack " CHANGE_ON GREEN TEXT_COLOR "<int>" CHANGE_ON BLUE TEXT_COLOR
           "       [0x%lx]" RESET " ____________________________ ", (uintptr_t)stk);
    printf(CHANGE_ON BOLD WITH LIGHT_RED TEXT_COLOR "ERROR! ");

    if (STATUS & BAD_VALUE)              printf("BAD_VALUE(%d) ",    BAD_VALUE);
    if (STATUS & BAD_START_CAPACITY)     printf("BAD_START_CAPACITY(%d) ",    BAD_START_CAPACITY);
    if (STATUS & BAD_STK_PTR)  {
        printf("BAD_STK_PTR(%d) ", BAD_STK_PTR);
        printf(RESET "\n");
        return;
    }

    if (STATUS & BAD_SIZE_R)        printf("BAD_SIZE_RIGHT(%d) ", BAD_SIZE_R);
    if (STATUS & BAD_SIZE_L)        printf("BAD_SIZE_LEFT(%d) ", BAD_SIZE_L);
    if (STATUS & BAD_STK_CAPACITY)  printf("BAD_STK_CAPACITY(%d) ", BAD_STK_CAPACITY);
    if (STATUS & BAD_DATA_PTR)      printf("BAD_DATA_PTR(%d) ", BAD_DATA_PTR);
    if (STATUS & LEFT_CANARY_DEAD)  printf("LEFT_CANARY_DEAD(%d) ", LEFT_CANARY_DEAD);
    if (STATUS & RIGHT_CANARY_DEAD) printf("RIGHT_CANARY_DEAD(%d) ", RIGHT_CANARY_DEAD);
    printf(RESET "\n");
    print_data(stk, STATUS);
}

void print_data(stack_s* stk, int STATUS)
{
    printf(RESET "\n");
        printf("{\n");
        printf(CHANGE_ON GREEN TEXT_COLOR "    size       " CHANGE_ON BLUE TEXT_COLOR  "%zu"  RESET "\n", stk->size - 1);
        printf(CHANGE_ON GREEN TEXT_COLOR "    capacity   " CHANGE_ON BLUE TEXT_COLOR  "%zu" RESET "\n", stk->capacity);
        printf(CHANGE_ON GREEN TEXT_COLOR "    data       " CHANGE_ON BLUE TEXT_COLOR  "[0x%lx]" RESET "\n", (uintptr_t)stk->data);
    if (!(STATUS & BAD_DATA_PTR || STATUS & BAD_STK_CAPACITY)) {
        printf("    {\n");
        for (size_t i = 0; i < stk->capacity + 2; i++) {
            if (stk->data[i] == POISON)
                printf(CHANGE_ON CYAN  TEXT_COLOR  "        [%zu] = %d,         (POISON)\n" RESET, i, stk->data[i]);
            if (stk->data[i] == L_CANARY || stk->data[i] == R_CANARY)
                printf(CHANGE_ON PURPLE TEXT_COLOR "        [%zu] = %d,         (CANARY)\n" RESET, i, stk->data[i]);
            else if (stk->data[i] != POISON )
                printf(CHANGE_ON YELLOW TEXT_COLOR "       *[%zu] = %d,         (VALUE) \n" RESET, i, stk->data[i]);
        }
    printf("    }\n\n");
    }
    printf("}\n");
}


#endif // MY_STACK
