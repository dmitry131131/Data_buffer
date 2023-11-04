/**
 * @file
 * @brief Data buffer functions
*/
#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

enum fileCreateMode {
    BIN,
    TEXT
};

struct outputBuffer {
    char* Buffer;
    size_t bufferPointer;
};

int buffer_ctor(outputBuffer* buffer, size_t size);

int buffer_dtor(outputBuffer* buffer);

int create_output_file(FILE** file, const char* filename, fileCreateMode modeCode);

int write_char_to_buffer(outputBuffer* buffer, unsigned char num);

int write_int_to_buffer(outputBuffer* buffer, int num);

int write_double_to_buffer(outputBuffer* buffer, double num);

int write_buffer_to_file(FILE* file, outputBuffer* buffer);

#endif