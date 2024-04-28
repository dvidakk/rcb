#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

#endif // TEST_H