#include "io.h"

#include <stdio.h>
#include <stdlib.h>

#include "../../thirdparty/c_log.h"

char *io_read_file(char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        c_log(C_LOG_SEVERITY_ERROR, "Can't read file '%s'", path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file
    char *buffer = (char*) malloc(sizeof(char) * (length + 1));
    if (!buffer) {
        c_log(C_LOG_SEVERITY_ERROR, "Can't allocate buffer of size %lu", length);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);

    return buffer;
}
