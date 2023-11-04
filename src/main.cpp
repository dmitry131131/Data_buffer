#include <stdio.h>
#include "DataBuffer.h"

int main()
{
    outputBuffer buff = {};

    buffer_ctor(&buff, 30);

    FILE* file = NULL;
    create_output_file(&file, "log.txt", TEXT);

    write_double_to_buffer(&buff, 4.5);

    write_buffer_to_file(file, &buff);

    buffer_dtor(&buff);

    return 0;
}