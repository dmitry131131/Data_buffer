/**
 * @file
 * @brief Data buffer functions sources
*/
#include <stdio.h>
#include <sys/stat.h>
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

static dataBufferErrorCode custom_buffer_realloc(outputBuffer* buffer);

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
    va_list fakeArguments;
    va_start(arguments, format);
    va_start(fakeArguments, format);
    
    size_t stringLen = (size_t) vsnprintf(NULL, 0, format, fakeArguments);

    if (buffer->bufferPointer >= (BUFFER_SIZE - stringLen))
    {
        if (buffer->mode == DYNAMIC)
        {
            if (custom_buffer_realloc(buffer))
            {
                print_buffer_error(ALLOC_MEMORY_ERROR);
                return 1;
            }
        }
        else
        {
            if (buffer->AUTO_FLUSH)
            {
                write_buffer_to_file(buffer, buffer->filePointer);
                clean_buffer(buffer);
            }
            else
            {
                return 1;
            }
        }
    }
    
    buffer->bufferPointer += (size_t) vsprintf(buffer->Buffer + buffer->bufferPointer, format, arguments);

    return 0;
}

static dataBufferErrorCode custom_buffer_realloc(outputBuffer* buffer)
{
    assert(buffer);

    buffer->customBuffer = (char*) realloc(buffer->customBuffer, buffer->customSize * 2);

    if (!(buffer->customBuffer))
    {
        return ALLOC_MEMORY_ERROR;
    }

    return NO_DATA_BUFFER_ERRORS;
}

int write_char_to_buffer(outputBuffer* buffer, unsigned char num)
{
    assert(buffer);

    if (buffer->bufferPointer >= (BUFFER_SIZE) - sizeof(char))
    {
        if (buffer->AUTO_FLUSH)
        {
            write_buffer_to_file(buffer, buffer->filePointer);
            clean_buffer(buffer);
        }
        else
        {
            return 1;
        }
    }

    if (buffer->mode == STATIC)
        (buffer->Buffer)[buffer->bufferPointer] = (char) num;
    else if (buffer->mode == DYNAMIC)
        (buffer->customBuffer)[buffer->bufferPointer] = (char) num;
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
            write_buffer_to_file(buffer, buffer->filePointer);
            clean_buffer(buffer);
        }
        else
        {
            return 1;
        }
    }

    char* bufferPtr = (buffer->mode)? buffer->customBuffer : buffer->Buffer;

    for (size_t i = 0; i < sizeof(int); i++)
    {
        (bufferPtr)[buffer->bufferPointer + i] = intPtr[i];
    }

    buffer->bufferPointer += sizeof(int);

    return 0;
}

int write_chars_to_buffer(outputBuffer* buffer, size_t n, ...)
{
    assert(buffer);

    va_list factor;     
    va_start(factor, n);   
    
    for (size_t i = 0; i < n; i++)
    {
        write_char_to_buffer(buffer, va_arg(factor, unsigned int));
    }

    va_end(factor);
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
            write_buffer_to_file(buffer, buffer->filePointer);
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

    char* bufferPtr = (buffer->mode)? buffer->customBuffer : buffer->Buffer;

    FILE* filePtr = (file)? file : buffer->filePointer;

    if (fwrite(bufferPtr, sizeof(char), buffer->bufferPointer, filePtr) != buffer->bufferPointer)
    {
        RET_ERR(FWRITE_ERROR);
    }

    return 0;
}

int clean_buffer(outputBuffer* buffer)
{
    assert(buffer);

    if (buffer->mode == STATIC)
    {
        memset(buffer->Buffer, 0, BUFFER_SIZE);
        buffer->bufferPointer = 0;
    }

    return 0;
}

int read_file_in_buffer(outputBuffer* buffer, FILE* file)
{
    assert(buffer);
    assert(file);

    struct stat buff = {};
    if (fstat(fileno(file), &buff))
    {
        return 1;
    }
    if (buff.st_size < 0) return 1;

    if (buffer_ctor(buffer, (size_t) buff.st_size))
    {
        return 1;
    }

    if (fread(buffer->customBuffer, sizeof(char), buffer->customSize, file) != buffer->customSize)
    {
        return 1;
    }

    return 0;
}

#undef RET_ERR