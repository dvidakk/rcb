#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "base64.h"

char* base64_encode(const char* data) {

    size_t input_length = strlen(data);
    size_t output_length = 4 * ((input_length + 2) / 3);

    char* encoded_data = malloc(output_length + 1); // +1 for null-termination
    if (encoded_data == NULL) return NULL;

    int j = 0;
    for (size_t i = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = b64chars[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = b64chars[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = b64chars[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = b64chars[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < 3 - input_length % 3; i++) {
        encoded_data[output_length - 1 - i] = '=';
    }

    encoded_data[output_length] = '\0'; // null-terminate the string

    return encoded_data;
}