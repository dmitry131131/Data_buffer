/**
 * @file
 * @brief Data buffer functions sources
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>

#include "DataBufferErrors.h"
#include "DataBuffer.h"

#define RET_ERR(code) do{           \
    print_buffer_error(code);       \
    return 1;                       \
}while(0)

static dataBufferErrorCode create_buffer(char** buffer, size_t size);

int buffer_ctor(outputBuffer* buffer, size_t size)
{
    assert(buffer);

    buffer->customBuffer  = NULL;
    buffer->customSize    = size;
    buffer->bufferPointer = 0;
    buffer->mode          = DYNAMIC;

    dataBufferErrorCode err = NO_DATA_BUFFER_ERRORS;
    if ((err = create_buffer(&(buffer->customBuffer), size)))
    {
        RET_ERR(err);
    }

    return 0;
}

int buffer_dtor(outputBuffer* buffer)
{
    assert(buffer);

    free(buffer->customBuffer);
    buffer->customBuffer  = NULL;
    buffer->customSize    = 0;
    buffer->bufferPointer = 0;
    buffer->mode          = STATIC;

    return 0;
}


int create_output_file(FILE** file, const char* filename, fileCreateMode modeCode)
{
    assert(filename);

    char mode[10] = {};
    mode[0] = 'w';
    switch (modeCode)
    {
    case BIN:
        mode[1] = 'b';
        break;

    case TEXT:
        break;
    
    default:
        RET_ERR(WRONG_FILE_CREATE_MODE);
    }

    *file = fopen(filename, mode);
    if (!file)
    {
        RET_ERR(FILE_CREATING_ERROR);
    }

    return 0;
}

static dataBufferErrorCode create_buffer(char** buffer, size_t size)
{
    assert(buffer);

    *buffer = (char*) calloc(size + 1, sizeof(char));

    if (!buffer)
    {
        return ALLOC_MEMORY_ERROR;
    }

    return NO_DATA_BUFFER_ERRORS;
}

int print_to_buffer(outputBuffer* buffer, const char* format, ...)
{
    assert(buffer);
    assert(format);

    va_list arguments;
    va_start(arguments, format);

    size_t stringLen = (size_t) vsnprintf(NULL, 0, format, arguments);

    if (buffer->bufferPointer >= (BUFFER_SIZE) - stringLen)
    {
        if (buffer->AUTO_FLUSH)
        {
            fwrite(buffer->Buffer, sizeof(char), BUFFER_SIZE, buffer->filePointer);
            clean_buffer(buffer);
        }
        else
        {
            return 1;
        }
    }

    buffer->bufferPointer += (size_t) vsprintf(buffer->Buffer + buffer->bufferPointer, format, arguments);
    return 0;
}

int write_char_to_buffer(outputBuffer* buffer, unsigned char num)
{
    assert(buffer);

    if (buffer->bufferPointer >= (BUFFER_SIZE) - sizeof(char))
    {
        if (buffer->AUTO_FLUSH)
        {
            fwrite(buffer->Buffer, sizeof(char), BUFFER_SIZE, buffer->filePointer);
            clean_buffer(buffer);
        }
        else
        {
            return 1;
        }
    }

    (buffer->Buffer)[buffer->bufferPointer] = (char) num;
    (buffer->bufferPointer)++;

    return 0;
}

int write_int_to_buffer(outputBuffer* buffer, int num)
{
    assert(buffer);

    char* intPtr = (char*) &num;

    if (buffer->bufferPointer >= (BUFFER_SIZE) - sizeof(int))
    {
        if (buffer->AUTO_FLUSH)
        {
            fwrite(buffer->Buffer, sizeof(char), BUFFER_SIZE, buffer->filePointer);
            clean_buffer(buffer);
        }
        else
        {
            return 1;
        }
    }

    for (size_t i = 0; i < sizeof(int); i++)
    {
        (buffer->Buffer)[buffer->bufferPointer + i] = intPtr[i];
    }

    buffer->bufferPointer += sizeof(int);

    return 0;
}

int write_double_to_buffer(outputBuffer* buffer, double num)
{
    assert(buffer);

    char* doublePtr = (char*) &num;

    if (buffer->bufferPointer >= (BUFFER_SIZE) - sizeof(int))
    {
        if (buffer->AUTO_FLUSH)
        {
            fwrite(buffer->Buffer, sizeof(char), BUFFER_SIZE, buffer->filePointer);
            clean_buffer(buffer);
        }
        else
        {
            return 1;
        }
    }

    for (size_t i = 0; i < sizeof(double); i++)
    {
        (buffer->Buffer)[buffer->bufferPointer + i] = doublePtr[i];
    }

    buffer->bufferPointer += sizeof(double);

    return 0;
}

int write_buffer_to_file(outputBuffer* buffer, FILE* file)
{
    assert(buffer);

    if (fwrite(buffer->Buffer, sizeof(char), buffer->bufferPointer, file) != buffer->bufferPointer)
    {
        RET_ERR(FWRITE_ERROR);
    }

    return 0;
}

int clean_buffer(outputBuffer* buffer)
{
    assert(buffer);

    memset(buffer->Buffer, 0, BUFFER_SIZE);
    
    buffer->bufferPointer = 0;

    return 0;
}

#undef RET_ERR