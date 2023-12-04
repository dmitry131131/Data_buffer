/**
 * @file
 * @brief Data buffer errors functions and enums
*/
#ifndef DATA_BUFFER_ERRORS_H
#define DATA_BUFFER_ERRORS_H

enum dataBufferErrorCode {
    NO_DATA_BUFFER_ERRORS,
    ALLOC_MEMORY_ERROR,
    WRONG_FILE_CREATE_MODE,
    FILE_CREATING_ERROR,
    FWRITE_ERROR,
    INPUT_FILE_DESCRIPTOR_ERROR
};

void print_buffer_error(dataBufferErrorCode error);

void print_buffer_error_message(dataBufferErrorCode error, FILE* stream);

#endif