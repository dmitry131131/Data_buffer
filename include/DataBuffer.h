/**
 * @file
 * @brief Data buffer functions
*/
#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

const size_t BUFFER_SIZE = 4000;

enum fileCreateMode {
    BIN,
    TEXT
};

struct outputBuffer {
    char Buffer[BUFFER_SIZE];
    size_t bufferPointer;
    FILE* filePointer;
    bool AUTO_FLUSH;
};

int buffer_ctor(outputBuffer* buffer, size_t size);

int buffer_dtor(outputBuffer* buffer);

int create_output_file(FILE** file, const char* filename, fileCreateMode modeCode);

int print_to_buffer(outputBuffer* buffer, const char* format, ...);

int write_char_to_buffer(outputBuffer* buffer, unsigned char num);

int write_int_to_buffer(outputBuffer* buffer, int num);

int write_double_to_buffer(outputBuffer* buffer, double num);

int write_buffer_to_file(outputBuffer* buffer, FILE* file = nullptr);

int clean_buffer(outputBuffer* buffer);

#endif