#define _CRT_SECURE_NO_WARNINGS
#include "utils/safe_func.h"

#include <string.h>

// Safe version of strncat that ensures the destination buffer is null-terminated
void safe_strncat(char **dest, const char *src, size_t dest_size) {
    size_t current_length = strlen(*dest);
    size_t remaining_buffer_space = dest_size - current_length - 1;

    if (strlen(src) < remaining_buffer_space) {
        strncat(*dest, src, strlen(src));
    } else {
        strncpy(*dest + current_length, src, remaining_buffer_space);
        (*dest)[dest_size - 1] = '\0'; // Ensure null-termination
    }
}