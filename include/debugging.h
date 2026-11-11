#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>
#include <errno.h>


#define DEBUG                                // Закомментируйте для отключения DEBUG режима


#ifdef DEBUG
    #define ON_DBG(...) __VA_ARGS__
#else
    #define ON_DBG(...)
#endif


#define PRINT_ERROR(error)                                                                                      \
    fprintf(stderr, "%s:%d " COLOR_TEXT("ERROR CODE: ", RED_D) "%s   " COLOR_TEXT("ERRNO: ", RED_D) "%s  %s\n", \
    __FILE__, __LINE__, my_str_error(error), strerror(errno), __FUNCTION__)

#define ASSERT_FOR_ARR(ind, size) assert(0 <= (size_t)ind && (size_t)ind < (size_t)size)

#define SAFE_CALL(fun_call, free_funcs) {                                                   \
        error_code_e safe_call_err = fun_call;                                              \
        if (safe_call_err != SUCCESS) {                                                     \
            LOG_ERROR("ERROR IN SAFE CALL %s: %s", #fun_call, my_str_error(safe_call_err)); \
            free_funcs;                                                                     \
            return safe_call_err;                                                           \
        }                                                                                   \
    }

#define $ANCHOR         fprintf(stderr, "%s:%d " COLOR_TEXT("ANCHOR",         VIOLET_D) " %s\n", __FILE__, __LINE__, __FUNCTION__)
#define $START_FUNCTION fprintf(stderr, "%s:%d " COLOR_TEXT("START FUNCTION", BLUE_D)   " %s\n", __FILE__, __LINE__, __FUNCTION__)
#define $END_FUNCTION   fprintf(stderr, "%s:%d " COLOR_TEXT("END FUNCTION",   YELLOW_D) " %s\n", __FILE__, __LINE__, __FUNCTION__)

#define RED_D    "91"
#define GREEN_D  "92"
#define BLUE_D   "94"
#define YELLOW_D "33"
#define VIOLET_D "35"

#define COLOR_TEXT(STR, COLOR)  "\033[" COLOR "m" STR "\033[0m"

enum error_code_e {
    SUCCESS              = 0,
    INCORRECT_SIZE       = 1,
    NULL_PARAM           = 2,
    ALLOCATION_ERROR     = 3,
    ERROR_DURING_OPEN    = 4,
    ERROR_DURING_CLOSE   = 5,
    CANARY_IS_DEAD       = 6,
    REINITIALIZATION     = 7,
    HASH_CHANGED         = 8,
    SEG_FAULT            = 9,
    ERROR_DURING_READ    = 10,
    DIVISION_BY_ZERO     = 11,
    STAT_READ_ERROR      = 12,
    END_OF_FILE          = 13,
    INCORRECT_PARAM      = 14,
    DONT_OPEN_LOG        = 15,
    NULL_STACK           = 16,
    ZERO_CAPACITY        = 17,
    SIZE_HIGHER_CAPACITY = 18,
    POP_VOID_STACK       = 19,
    INIT_VALUE           = -1,
    UNEXPECTED_ERROR     = -2,
};

#define       BGCOLOR_HX "#1d2021"
#define        BORDER_HX "#cbd5e1"
#define           RED_HX "#ea6962"
#define      DARK_RED_HX "#750e15"
#define         GREEN_HX "#a9b665"
#define      LAVENDER_HX "#d3869b"
#define     DARK_WINE_HX "#352932"
#define          AQUA_HX "#7daea3"
#define     DARK_TEAL_HX "#243332"
#define    MUTED_GRAY_HX "#40b329"
#define      GRAPHITE_HX "#385c0a"
#define     WARM_GRAY_HX "#7c6f64"
#define  HONEY_YELLOW_HX "#d8a657"
#define PASTEL_PURPLE_HX "#d3869b"
#define   OLIVE_GREEN_HX "#a9b665"
#define     MAIN_TEXT_HX "#ebdbb2"

#define LOG_ERROR(message, ...) _log("ERROR", __FILE__, __func__, __LINE__, message, RED_HX      __VA_OPT__(,) __VA_ARGS__)
#define    LOG_OK(message, ...) _log("OK",    __FILE__, __func__, __LINE__, message, GREEN_HX    __VA_OPT__(,) __VA_ARGS__)
#define       LOG(message, ...) _log("INFO",  __FILE__, __func__, __LINE__, message, LAVENDER_HX __VA_OPT__(,) __VA_ARGS__)

extern FILE* GLOBAL_LOG_FILE;
error_code_e log_start();
void _log(const char*  prefix, const char* file,    const char* function,
                size_t line,   const char* message, const char* color    ...);
error_code_e log_end  ();


unsigned long djb2_hash    (const unsigned char* str, const size_t size);
unsigned long djb2_str_hash(const unsigned char* str);

const char* my_str_error(const error_code_e error_code);

error_code_e is_readble_ptr  (const void*       ptr);
error_code_e stat_get_size   (const char* const file_name, size_t* const file_size);
error_code_e stat_get_blksize(const char* const file_name, size_t* const file_size);

#define $(param) {$ANCHOR; fprintf(stderr, COLOR_TEXT(#param, VIOLET_D) " = "); _$(param);}
void _$(const int           num);
void _$(const double        num);
void _$(const unsigned long num);
void _$(const long long     num);
void _$(const void* const   ptr);
void _$(const char* const   str);
void _$(const char          symbol);
void _$(const long          num);

#endif
