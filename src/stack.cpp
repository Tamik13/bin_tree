#include <stdio.h>
#include <assert.h>

#include "../include/stack.h"


error_code_e stack_verify(stack_s* const stack) {
    if (stack == NULL) {
        LOG_ERROR("stack is NULL");
        return NULL_STACK;
    }

    ON_DBG(
    if (is_readble_ptr(stack)) {
        LOG_ERROR("stack ptr cant be read");
        return SEG_FAULT;
    }
    )

    if (stack->data == NULL) {
        LOG_ERROR("stack->data is NULL");
        return NULL_STACK;
    }

    ON_DBG(
    if (stack->canary_data == NULL) {
        LOG_ERROR("stack->canary_data is NULL");
        return NULL_STACK;
    }

    if (stack->left_canary != LEFT_CANARY) {
        LOG_ERROR("first canary in struct IS DEAD(((");
        stack_dmp(stack);
        return CANARY_IS_DEAD;
    }

    if (stack->right_canary != RIGHT_CANARY) {
        LOG_ERROR("second canary in struct IS DEAD(((");
        stack_dmp(stack);
        return CANARY_IS_DEAD;
    }

    if (is_readble_ptr(stack->data)) {

        LOG_ERROR("stack_verify: ERROR stack->data ptr cant be read");

        return SEG_FAULT;
    }

    if (stack->canary_data[0] != LEFT_CANARY) {
        LOG_ERROR("first canary in data IS DEAD(((");
        stack_dmp(stack);
        return CANARY_IS_DEAD;
    }

    if (stack->data[stack->capacity] != RIGHT_CANARY) {
        LOG_ERROR("stack_verify: ERROR second canary in data IS DEAD(((");
        stack_dmp(stack);
        return CANARY_IS_DEAD;
    }
    )

    if (stack->capacity == 0) {
        LOG_ERROR("zero capacity");
        stack_dmp(stack);
        return ZERO_CAPACITY;
    }

    if (stack->capacity < stack->size) {
        LOG_ERROR("size higher capacity");
        stack_dmp(stack);
        return SIZE_HIGHER_CAPACITY;
    }

    ON_DBG(
    unsigned long old_struct_hash = stack->struct_hash; // TODO: define
    unsigned long old_data_hash   = stack->data_hash;
    stack->       struct_hash     = 0;
    stack->       data_hash       = 0;
    unsigned long new_struct_hash = djb2_hash((const unsigned char*)stack, sizeof(*stack));

    if (old_struct_hash != new_struct_hash) {
        LOG_ERROR("new_hash != old_hash");
        stack_dmp(stack);
        return HASH_CHANGED;
    }

    unsigned long new_data_hash   = djb2_hash((const unsigned char*)stack->canary_data, sizeof(stack_element_t) * ((size_t)stack->capacity + COUNT_CANARY));
    stack->struct_hash = new_struct_hash;

    if (old_data_hash != new_data_hash) {
        LOG_ERROR("new_hash != old_hash");
        stack_dmp(stack);
        return HASH_CHANGED;
    }

    stack->data_hash = new_data_hash;
    )

    return SUCCESS;
}


error_code_e stack_init(stack_s* const stack, const ssize_t capacity ON_DBG(, const char* const name, const char* const file, const char* const function, const size_t line)) {
    if (capacity == 0) {
        LOG_ERROR("try to init stack with zero capacity");
        return ZERO_CAPACITY;
    }

    if (is_stack_init(stack)) {
        LOG_ERROR("try to init already initialized stack");
        return REINITIALIZATION;
    }

    stack->data = (stack_element_t*)calloc((size_t)capacity ON_DBG(+ COUNT_CANARY * CANARY_SIZE), sizeof(stack_element_t));

    if (stack->data == NULL) {
        LOG_ERROR("allocation error");
        return ALLOCATION_ERROR;
    }

    stack->capacity = capacity;
    stack->size     = 0;

    ON_DBG(
        stack->canary_data =  stack->data;
        stack->data        += COUNT_LEFT_CANARY * CANARY_SIZE;
        stack->name        =  name;
        stack->file        =  file;
        stack->function    =  function;
        stack->line        =  line;
    )

    for (ssize_t ind = 0; ind < stack->capacity; ind++) {
        ASSERT_FOR_ARR(ind, stack->capacity); // TODO malloc_size
        stack->data[ind] = POISON;
    }

    ON_DBG(
    stack->canary_data[0]        = LEFT_CANARY;
    stack->data[stack->capacity] = RIGHT_CANARY;
    stack->left_canary           = LEFT_CANARY;
    stack->data[stack->capacity] = RIGHT_CANARY;
    stack->struct_hash = 0;
    stack->struct_hash = djb2_hash((const unsigned char*)stack, sizeof(*stack));

    stack->data_hash = 0;
    stack->data_hash = djb2_hash((const unsigned char*)stack->canary_data, sizeof(stack_element_t) * ((size_t)stack->capacity + COUNT_CANARY));
    )

    return SUCCESS;
}


error_code_e stack_push(stack_s* const stack, const stack_element_t value) {
    SAFE_CALL( stack_verify(stack), )

    if (stack->size == stack->capacity) {
        SAFE_CALL( stack_recalloc(stack, stack->size * HIGHER_COEF), )
    }

    stack->data[stack->size++] = value;

    ON_DBG(
    stack->struct_hash = 0;
    stack->data_hash   = 0;

    stack->struct_hash = djb2_hash((const unsigned char*)stack, sizeof(*stack));
    stack->data_hash   = djb2_hash((const unsigned char*)stack->canary_data, sizeof(stack_element_t) * ((size_t)stack->capacity + COUNT_CANARY));
    )

    SAFE_CALL( stack_verify(stack), )

    LOG_OK("stack push elem successfully value = " STK_MODIFIER " size = %zd capacity = %zd", value, stack->size, stack->capacity);
    return SUCCESS;
}


error_code_e stack_pop(stack_s* const stack, stack_element_t* const value) {
    SAFE_CALL( stack_verify(stack), )

    if (stack->size == 0) {
        LOG_ERROR("try to pop void stack");
        stack_dmp(stack);
        return POP_VOID_STACK;
    }

    if (value == NULL) {
        LOG_ERROR("null value ptr");
        stack_dmp(stack);
        return NULL_PARAM;
    }

    if (stack->size <= stack->capacity / LOWER_COEF && stack->capacity / LOWER_COEF ) {
        SAFE_CALL( stack_recalloc(stack, stack->capacity / LOWER_COEF), )
    }

    stack->size--;
    *value = stack->data[stack->size];
    stack->data[stack->size] = POISON;

    ON_DBG(
    stack->struct_hash = 0;
    stack->data_hash   = 0;

    stack->struct_hash = djb2_hash((const unsigned char*)stack, sizeof(*stack));
    stack->data_hash   = djb2_hash((const unsigned char*)stack->canary_data, sizeof(stack_element_t) * ((size_t)stack->capacity + COUNT_CANARY));
    )

    SAFE_CALL( stack_verify(stack), )


    LOG_OK("stack pop  elem successfully value = " STK_MODIFIER " size = %zd capacity = %zd", *value, stack->size, stack->capacity);
    return SUCCESS;
}


error_code_e stack_destroy(stack_s* const stack) {
    SAFE_CALL( stack_verify(stack), )

    free(stack->data ON_DBG( - COUNT_LEFT_CANARY * CANARY_SIZE));

    stack->data       = NULL;
    stack->capacity   = 0;
    stack->size       = 0;

    ON_DBG(
        stack->canary_data = NULL;
        stack->struct_hash = 0;
        stack->data_hash   = 0;
        stack->name        = NULL;
        stack->file        = NULL;
        stack->function    = NULL;
        stack->line        = 0;
    )

    return SUCCESS;
}


error_code_e stack_recalloc(stack_s* const stack, const ssize_t new_capacity) {
    SAFE_CALL( stack_verify(stack), )

    stack_element_t* new_canary_data = (stack_element_t*)realloc((void*)(stack->data ON_DBG( - COUNT_LEFT_CANARY * CANARY_SIZE)), ((size_t)new_capacity ON_DBG(+ COUNT_CANARY)) * sizeof(stack_element_t));

    if (new_canary_data == NULL) {
        LOG_ERROR("allocation error");
        return ALLOCATION_ERROR;
    }

    stack->data = new_canary_data;

    ON_DBG(
        stack->canary_data        = new_canary_data;
        stack->data               += COUNT_LEFT_CANARY * CANARY_SIZE;

        stack->canary_data[0]     = LEFT_CANARY;
        stack->data[new_capacity] = RIGHT_CANARY;
    )

    for (ssize_t ind = stack->size; ind < new_capacity; ind++) {
        stack->data[ind] = POISON;
    }

    stack->capacity = new_capacity;

    ON_DBG(
    stack->struct_hash = 0;
    stack->data_hash   = 0;

    stack->struct_hash = djb2_hash((const unsigned char*)stack, sizeof(*stack));
    stack->data_hash   = djb2_hash((const unsigned char*)stack->canary_data, sizeof(stack_element_t) * ((size_t)stack->capacity + COUNT_CANARY));
    )

    SAFE_CALL( stack_verify(stack), )


    return SUCCESS;
}


bool is_stack_init(const stack_s* const stack) {
    assert(stack != NULL);

    ON_DBG(
    if (stack->file != NULL) {
        return true;
    }

    if (stack->function != NULL) {
        return true;
    }

    if (stack->line != 0) {
        return true;
    }

    if (stack->name != NULL) {
        return true;
    }

    if (stack->canary_data != NULL) {
        return true;
    }
    )

    if (stack->data != NULL) {
        return true;
    }

    if (stack->capacity != 0) {
        return true;
    }

    if (stack->size != 0) {
        return true;
    }

    return false;
}


void stack_dmp(const stack_s* const stack) {
    ON_DBG( LOG("stack_s \"%s\"[%p] created by %s() at %s:%zu", stack->name, stack, stack->function, stack->file, stack->line);)

    if (stack == NULL) {
        LOG_ERROR("NULL STACK CANT BE DUMP");
    }

    LOG("{");
    LOG("\tsize     = %zu", stack->size);
    LOG("\tcapacity = %zu", stack->capacity);

    ON_DBG(
    LOG("\thash     = %lu", stack->struct_hash);

    LOG("\tcanary_data[%p]", stack->canary_data);
    LOG("\t{");

    LOG("\t\t [%zu] = " POISON_MODIFIER " (CANARY!!!)", (size_t)0, stack->canary_data[0]);

    for (ssize_t ind = 0; ind < stack->size; ind++) {
        LOG("\t\t*[%zu] = " STK_MODIFIER, ind + 1, stack->data[ind]);
    }

    for (ssize_t ind = stack->size; ind < stack->capacity; ind++) {
        LOG("\t\t [%zu] = " POISON_MODIFIER " (POISON!!!)", ind + 1, stack->data[ind]);
    }

    LOG("\t\t [%zu] = " POISON_MODIFIER " (CANARY!!!)", stack->capacity + 1, stack->data[stack->capacity]);
    LOG("\t}");
    )

    LOG("\tdata[%p]", stack->data);
    LOG("\t{");

    for (ssize_t ind = 0; ind < stack->size; ind++) {
        LOG("\t\t*[%zu] = " STK_MODIFIER, ind, stack->data[ind]);
    }

    for (ssize_t ind = stack->size; ind < stack->capacity; ind++) {
        LOG("\t\t [%zu] = " POISON_MODIFIER " (POISON!!!)", ind, stack->data[ind]);
    }

    LOG("\t}");
    LOG("}");
}
