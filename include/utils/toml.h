#ifndef TOML_H
#define TOML_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VALUE_STRING,
    VALUE_INTEGER,
    VALUE_FLOAT,
    VALUE_BOOLEAN,
    VALUE_DATE,
    VALUE_ARRAY,
    VALUE_TABLE
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        char *string;
        int integer;
        double float_;
        bool boolean;
        struct tm *date;
        struct {
            size_t size;
            struct Value **values;
        } array;
        struct {
            size_t size;
            char **keys;
            struct Value **values;
        } table;
    };
} Value;

typedef struct {
    size_t size;
    char **keys;
    Value **values;
} Document;

// create a tokenization function
// create a parser function


Document *toml_load(const char *filename);
Value *toml_get(Document *doc, const char *key);
void toml_set(Document *doc, const char *key, Value *value);
void toml_save(Document *doc, const char *filename);
void toml_free(Document *doc);

#ifdef __cplusplus
}
#endif

#endif // TOML_H
