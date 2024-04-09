#include <stdio.h>

#include "DataBuffer.h"

int main()
{
    outputBuffer buff = {};

    buffer_ctor(&buff, 30);

    FILE* file = NULL;
    create_output_file(&file, "log.dot", TEXT);

    print_to_buffer(&buff, "sdfsfsf[][]\n"
                           "sdcscsccscs");

    write_buffer_to_file(&buff, file);

    buffer_dtor(&buff);

    return 0;
}