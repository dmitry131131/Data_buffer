/**
 * @file
 * @brief Data buffer errors handling functions
*/
#include <stdio.h>

#include "Color_output.h"
#include "DataBufferErrors.h"

void print_buffer_error(dataBufferErrorCode error)
{
    print_buffer_error_message(error, stderr);
}

void print_buffer_error_message(dataBufferErrorCode error, FILE* stream)
{
    color_fprintf(stream, COLOR_RED, STYLE_BOLD, "Data Buffer error: ");

    #define CHECK_CODE(code, message)               \
        case code:                                  \
            fprintf(stream, message);               \
            break;                                  \

    switch (error)
    {
        case NO_DATA_BUFFER_ERRORS:
            break;

        CHECK_CODE(ALLOC_MEMORY_ERROR,          "Error in calloc, alloced memory for buffer!\n");
        CHECK_CODE(WRONG_FILE_CREATE_MODE,      "Wrong file creating mode given! Try to use TEXT or BIN mode.\n");
        CHECK_CODE(FILE_CREATING_ERROR,         "Error in file creating process!\n");
        CHECK_CODE(FWRITE_ERROR,                "Error in writing buffer to file!\n");

    default:
        fprintf(stream, "Unknown error!\n");
        break;
    }
    #undef CHECK_CODE
}