#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/ini_utils.h"

Creds* parseIniFile(char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return NULL;
    }

    char line[256];
    Creds* creds = malloc(sizeof(Creds));
    while (fgets(line, sizeof(line), file))
    {
        char* key = strtok(line, "=");
        char* value = strtok(NULL, "=");
        if (strcmp(key, "client_id") == 0)
        {
            // remove the newline character and the carriage return character
            value[strcspn(value, "\n\r")] = 0;
            creds->client_id = strdup(value);
        }
        else if (strcmp(key, "client_secret") == 0)
        {
            value[strcspn(value, "\n\r")] = 0;
            creds->client_secret = strdup(value);
        }
        else if (strcmp(key, "username") == 0)
        {
            value[strcspn(value, "\n\r")] = 0;
            creds->user = strdup(value);
        }
        else if (strcmp(key, "password") == 0)
        {
            value[strcspn(value, "\n\r")] = 0;
            creds->password = strdup(value);
        }
        else if (strcmp(key, "user_agent") == 0)
        {
            value[strcspn(value, "\n\r")] = 0;
            creds->user_agent = strdup(value);
        }
    }
    fclose(file);
    return creds;
}
