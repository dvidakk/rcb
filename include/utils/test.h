#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../matrix/models/messages.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct User {
    char *user_id;
    char *displayname;
    struct User *next;
} User;

unsigned int hash(char *key) {
    unsigned int hash = 0;
    while (*key) hash = (hash << 5) + *key++;
    return hash;
}
User **createUserTable(int size) {
    return (User**)calloc(size, sizeof(User*));
}

void insertUser(User **table, int size, char *user_id, char *displayname) {
    unsigned int index = hash(user_id) % size;
    User *user = (User*)malloc(sizeof(User));
    user->user_id = user_id;
    user->displayname = displayname;
    user->next = table[index];
    table[index] = user;
}

char *getUserDisplayname(User **table, int size, char *user_id) {
    unsigned int index = hash(user_id) % size;
    User *user = table[index];
    while (user) {
        if (strcmp(user->user_id, user_id) == 0) return user->displayname;
        user = user->next;
    }
    return NULL;
}

void freeUserTable(User **table, int size) {
    for (int i = 0; i < size; i++) {
        User *user = table[i];
        while (user) {
            User *next = user->next;
            free(user);
            user = next;
        }
    }
    free(table);
}

typedef struct {
    MessageResponse **responses;
    int size;
} MessageResponseList;

char* convertTimestampToLocaltime(double timestamp) {
    time_t time_t_val = (time_t)timestamp;
    struct tm *time_struct = localtime(&time_t_val);
    char *time_str = (char*)malloc(sizeof(char) * 100);
    strftime(time_str, 100, "%Y-%m-%d %H:%M:%S", time_struct);
    return time_str;
}

void printDevider() {
    printf("\n");
    for (int i = 0; i < 100; i++) {
        printf("-");
    }
    printf("\n");
}

#ifdef __cplusplus
}
#endif

#endif // TEST_H