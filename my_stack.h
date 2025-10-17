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
    ALL_OK              = 1 << 1,
    BAD_DATA_PTR        = 1 << 2,
    BAD_SIZE_L          = 1 << 3,
    BAD_SIZE_R          = 1 << 4,
    BAD_START_CAPACITY  = 1 << 5,
    BAD_STK_CAPACITY    = 1 << 6,
    BAD_STK_PTR         = 1 << 7,
    BAD_VALUE           = 1 << 8,
    LEFT_CANARY_DEAD    = 1 << 9,
    RIGHT_CANARY_DEAD   = 1 << 10,
    BAD_PRINT_DATA      = 1 << 11
} StackErr_t;

struct stack_s
{
    stack_t* data;
    size_t size;
    size_t capacity;
    int STATUS;
};

StackErr_t StackCtor_(stack_s* stk, size_t capacity, const char* file, int line);
StackErr_t StackPush_(stack_s* stk, stack_t value, const char* file, int line);
StackErr_t StackPop_(stack_s* stk, stack_t* value, const char* file, int line);
StackErr_t StackDtor_(stack_s* stk, const char* file, int line);

#define StackCtor(stk, capacity) StackCtor_(stk, capacity, __FILE__, __LINE__)
#define StackPush(stk, value) StackPush_(stk, value, __FILE__, __LINE__)
#define StackPop(stk, value) StackPop_(stk, value, __FILE__, __LINE__)
#define StackDtor(stk) StackDtor_(stk, __FILE__, __LINE__)

StackErr_t StackVerify(stack_s* stk, int STATUS); // todo rename (add Stack)
StackErr_t StackDump(stack_s* stk);
StackErr_t print_data(stack_s* stk);
void do_RCanaryErr(stack_s* stk);
void do_LCanaryErr(stack_s* stk);


#define START_OK                                                                                    \
    if (capacity > MAXSIZE) STATUS |= BAD_START_CAPACITY;                                           \
    if (capacity == 0)      STATUS |= BAD_START_CAPACITY;                                           \
    if (stk == NULL) {                                                                              \
        STATUS |= BAD_STK_PTR;                                                                      \
        return (StackErr_t) STATUS;                                                                 \
    }                                                                                               \
    if (STATUS != ALL_OK) {                                                                         \
        printf("\n" CHANGE_ON BOLD WITH LIGHT_RED TEXT_COLOR "StackDump() from %s at %s:%d\n" RESET,\
        __func__, file, line);                                                                      \
        stk->STATUS = STATUS;                                                                       \
        StackDump(stk);                                                                             \
        return (StackErr_t) STATUS;                                                                 \
    }

#define STACK_OK                                                                                    \
    STATUS = StackVerify(stk, STATUS);                                                              \
    if (STATUS != ALL_OK) {                                                                         \
        printf("\n" CHANGE_ON BOLD WITH LIGHT_RED TEXT_COLOR "StackDump() from %s at %s:%d\n" RESET,\
        __func__, file, line);                                                                      \
        if (STATUS & BAD_STK_PTR)  {                                                                \
            printf("BAD_STK_PTR(%d) ", BAD_STK_PTR);                                                \
            printf(RESET "\n");                                                                     \
            return  (StackErr_t) STATUS;                                                            \
        }                                                                                           \
        stk->STATUS = STATUS;                                                                       \
        stk->STATUS |= StackDump(stk);                                                              \
        return (StackErr_t) STATUS;                                                                 \
    }                                                                                               \
    stk->STATUS = STATUS;

#endif // MY_STACK
