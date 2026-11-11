#ifndef _STACK_H
#define _STACK_H

#include <string.h>
#include <time.h>

#include "debugging.h"
#include "bin_tree.h"

typedef node_s** stack_element_t;                      // Введите между typedef и stack_element_t тип данных стека
#define STK_MODIFIER "%llu"                           // Введите после stk модификатор вывода типа данных стека
#define POISON_MODIFIER "%llx"
const   stack_element_t POISON = (node_s**)0xBAADF00D; // Введите редко (желательно никогда не) встречающиеся значение в стеке


#define LOWER_COEF  4
#define HIGHER_COEF 2

const size_t        COUNT_CANARY      = 2;
const size_t        COUNT_LEFT_CANARY = 1;
const size_t        CANARY_SIZE       = 1;  // единица измерения - sizeof(stack_elemnet)
const stack_element_t LEFT_CANARY       = (node_s**)0xDEADBABE;
const stack_element_t RIGHT_CANARY      = (node_s**)0xBADCAFE;

#define TO_STR(val) #val

#define STACK_INIT(stack, capacity) stack_init(&stack, capacity ON_DBG(, #stack, __FILE__, __FUNCTION__, __LINE__))

struct stack_s {
    ON_DBG(stack_element_t left_canary = LEFT_CANARY;)

    ON_DBG(
        const char*    name;
        const char*    file;
        const char*    function;
        size_t         line;
        unsigned long  struct_hash;
        unsigned long  data_hash;
        stack_element_t* canary_data;
    )

    stack_element_t* data;
    ssize_t          size;
    ssize_t          capacity;

    ON_DBG(stack_element_t right_canary = RIGHT_CANARY;)
};

error_code_e stack_init    (stack_s* const stack, const ssize_t capacity ON_DBG(, const char* const name, const char* const file, const char* const function, const size_t line));
error_code_e stack_push    (stack_s* const stack, const stack_element_t  value);
error_code_e stack_pop     (stack_s* const stack, stack_element_t* const value);
error_code_e stack_destroy (stack_s* const stack);
error_code_e stack_recalloc(stack_s* const stack, const ssize_t new_capacity);

error_code_e stack_verify(stack_s* const stack);
void print_stack         (const stack_s* const stack);
bool is_stack_init       (const stack_s* const stack);

void stack_dmp(const stack_s* const stack);

#endif
