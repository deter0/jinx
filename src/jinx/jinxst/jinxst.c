#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "reader.h"
#include "src/jinx/jinxst/jinxst.h"
#include "src/jinx/jinxst/jinxstTable.h"
#include "src/jinx/jinxst/log.h"
#include "tokenizer.h"

Namespace *getNamespace(char *nameSpaceName) {
    for (size_t i = 0; i < namespaceCount; i++) {
        printf("%s == %s\n", namespaces[i].NamespaceName, nameSpaceName);
        if (strcmp(namespaces[i].NamespaceName, nameSpaceName) == 0) {
            return &namespaces[i];
        }
    }
    return NULL;
}

static size_t savedState = 0;
static size_t i = 0; 
TokenPool *pool;
Token *current;

#define SAVE_TOKEN_STATE() savedState = i;\
                            current = pool->Tokens[i]
#define REVERT_TOKEN_STATE() i = savedState;\
                            current = pool->Tokens[i]
#define NEXT_TOKEN() i++;\
                     current = pool->Tokens[i]
#define GOTO_TOKEN_INDEX(index) i = (index);\
                     current = pool->Tokens[i]
#define PANIC_IF_END() if (current->Type == TOKEN_EOF || current == NULL) { \
                            set_error_cat("JINXST"); \
                            fprintf(stderr, "Unexpected EOF.\n"); \
                            panic(false); \
                        }

int findTokenOfType(TokenPool *pool, size_t from, TokenType type) {
    for (size_t j = from; j < pool->Length; j++) {
        if (pool->Tokens[j]->Type == type)
            return (int)j;
    }
    return -1;
}

struct lineStr {
    char *lineStr;
    size_t index;
};

int get_line_str(char *src, size_t line, struct lineStr *out) {
    size_t counted = 0;
    for (size_t k = 0; k < strlen(src); k++) {
        if (src[k] == '\n') {
            counted++;
            if (counted == line - 1) {
                size_t end = 0;
                for (size_t j = k + 1; j < strlen(src); j++) {
                    if (src[j] == '\n') {
                        end = j;
                        break;
                    }
                }
                if (end == 0) {
                    end = strlen(src);
                }
                char *buffer = malloc(end - k);
                assert(buffer != NULL);
                for (size_t j = k + 1; j < end; j++) {
                    buffer[j - (k + 1)] = src[j];
                }
                out->lineStr = trim(buffer);
                out->index = end;
                return 0;
            }
        }
    }
    return 1;
}

static char *debugInfo() {
    size_t index = (size_t)current->ReadResult->ptr_start - (size_t)pool->src;
    size_t line = get_line(pool->src, index);

    struct lineStr data = {0};
    int status = get_line_str(pool->src, line, &data);
    assert(status != 1);

    char *out = malloc(strlen(data.lineStr) + 50);
    out[strlen(data.lineStr) + 50] = '\0';
    sprintf(out, "Line %zu: %s`%s`%s", line, TERM_RED(), data.lineStr, TERM_DEFAULT());
    // free(data);
    return out;
}

int parseColor(char *val, RGBA *out) {
	char *colorVal = val + 1;
	size_t elements = strlen(colorVal);
    if (val[0] == '#') {
        val++;
        if (elements == 8 || elements == 6) {
            int r = 0, g = 0, b = 0, a = 255;
            bool failed = false;
            if (elements == 8) {
                failed = sscanf(colorVal, "%02x%02x%02x%02x", &r, &g, &b, &a) != 4;
            } else if (elements == 6) {
                failed = sscanf(colorVal, "%02x%02x%02x", &r, &g, &b) != 3;
            }
            assert(!failed);

            out->r = r / 255.0f;
            out->g = g / 255.0f;
            out->b = b / 255.0f;
            out->a = a / 255.0f;
            return 0;
        } else {
            set_error_cat("[JINXST (Tokenizer)]");
            fprintf(stderr, "Error parsing color value: `%s` Expected 8 or 6 data elements got %zu\n", val, elements);
            panic(true);
        }
    } else {
        assert(false && "NOT IMPLEMENTED");
    }
	return 1;
}


// TODO: Seperate into functions
void jinxst(char *jinxstSrc) {
    assert(jinxstSrc != NULL);
    read_result_pool *res = read_string(jinxstSrc);
    for (size_t i = 0; i < res->length; i++) {
        printf("[%s]\n", get_value(res->results[i]));
    }
    pool = Tokenize(res, jinxstSrc);
    current = pool->Tokens[0];
    logTokenPool(stdout, pool);
    SAVE_TOKEN_STATE();
    while (current->Type != TOKEN_EOF) {
        SAVE_TOKEN_STATE();
        if (current->Type == TOKEN_WORD) {
            char *nameSpaceName = get_value(current->ReadResult);
            NEXT_TOKEN();
            PANIC_IF_END();
            int scopeOpen = findTokenOfType(pool, i, TOKEN_OPEN_SCOPE);
            if (scopeOpen != -1) {
                // printf("Found Open: %s\n", nameSpaceName);
                int scopeClose = findTokenOfType(pool, scopeOpen, TOKEN_CLOSE_SCOPE);
                if (scopeClose != -1) {
                    // printf("Found Close: %s\n", nameSpaceName);
                    // printf("%s\n", nameSpaceName);

                    char *inherits[MAX_INHERITS];
                    size_t inheritCount = 0;

                    SAVE_TOKEN_STATE();
                    while (i != (size_t)scopeOpen) {
                        if (current->Type == TOKEN_INHERIT) {
                            NEXT_TOKEN();
                            if (current->Type == TOKEN_WORD) {
                                inherits[inheritCount] = get_value(current->ReadResult);
                                inheritCount++;
                                if (inheritCount > MAX_INHERITS) {
                                    set_error_cat("[JINXST]");
                                    fprintf(stderr, "Max inherits (%d) surpassed for namespace `%s`\n", MAX_INHERITS, nameSpaceName);
                                    fprintf(stderr, "\t%s\n", debugInfo());
                                    panic(true);
                                }
                            } else {
                                set_error_cat("[JINXST]");
                                fprintf(stderr, "Expected TOKEN_WORD after TOKEN_INHERIT got %s\n", TOKEN_STRINGS[current->Type]);
                                
                                panic(true);
                            }
                        }
                        NEXT_TOKEN();
                    }
                    SAVE_TOKEN_STATE();
                    printf("Namespace `%s`, Inherits (%zu):\n", nameSpaceName, inheritCount);
                    printf("Namespaces: %zu\n", namespaceCount);
                    for (size_t j = 0; j < namespaceCount; j++) {
                        printf("\t%s\n", namespaces[j].NamespaceName);
                    }

                    Namespace *ns = calloc(1, sizeof(Namespace));
                    assert(ns != NULL);
                    ns->NamespaceName = strdup(nameSpaceName);
                    ns->Properties = ht_new();
                    free(nameSpaceName);

                    for (size_t j = 0; j < inheritCount; j++) {
                        Namespace *inheriting = getNamespace(inherits[j]);
                        if (inheriting == NULL) {
                            set_error_cat("[JINXST]");
                            fprintf(stderr, "Attempt to inherit a namespace that doesn't exist: `%s` | %s\n", inherits[j], debugInfo());
                            panic(true);
                        }
                        ns->Inherits[ns->InheritsLength++] = inheriting;
                    }


                    namespaces[namespaceCount] = *ns;
                    namespaceCount++;

                    printf("Added namespace: %s\n", ns->NamespaceName);
                    if (namespaceCount >= MAX_NAMESPACES) {
                        set_error_cat("[JINXST]");
                        fprintf(stderr, "MAX NAMESPACES EXCEEDED: %s\n", debugInfo());
                        panic(true);
                    }
                    assert(i == (size_t)scopeOpen);
                    NEXT_TOKEN();
                    // printf("%s\n", get_value(current->ReadResult));

                    while (i != (size_t)scopeClose) {
                        if (current->Type == TOKEN_VARIABLE) {
                            SAVE_TOKEN_STATE();
                            NEXT_TOKEN();
                            PANIC_IF_END();
                            char *variableName = get_value(current->ReadResult);
                            if (current->Type != TOKEN_WORD) {
                                set_error_cat("[JINXST]");
                                fprintf(stderr, "Variable name expected after `$` got <%s>`%s`\n", TOKEN_STRINGS[current->Type], variableName);
                                fprintf(stderr, "\t%s\n", debugInfo());
                                panic(true);
                            }
                            int eqIndex = findTokenOfType(pool, i, TOKEN_OP_EQ);
                            if (eqIndex == -1) {
                                set_error_cat("[JINXST]");
                                fprintf(stderr, "Jinxst variables cannot be uninitalized no `=` found for variable `%s`\n", variableName);
                                fprintf(stderr, "\t%s\n", debugInfo());
                                panic(true);
                            }
                            bool override = false;
                            char *type = NULL;
                            while (i < (size_t)eqIndex) {
                                NEXT_TOKEN();
                                PANIC_IF_END();
                                // printf("%s\n", get_value(current->ReadResult));
                                if (current->Type == TOKEN_TYPE_ASSIGN) {
                                    NEXT_TOKEN();
                                    PANIC_IF_END();
                                    if (current->Type <= TOKEN_TYPE_COLOR && current->Type >= TOKEN_TYPE_VOID) {
                                        type = get_value(current->ReadResult);
                                    } else {
                                        set_error_cat("[JINXST]");
                                        fprintf(stderr, "Type expected after `:` for variable `%s` got <%s>`%s`\n", variableName, TOKEN_STRINGS[current->Type], get_value(current->ReadResult));
                                        fprintf(stderr, "\t%s\n", debugInfo());
                                        panic(true);
                                    }
                                } else if (current->Type == TOKEN_OVERRIDE) {
                                    override = true;
                                } else if (current->Type == TOKEN_OP_EQ) {
                                    break;
                                } else {
                                    set_error_cat("[JINXST]");
                                    fprintf(stderr, "Unexpected token: <%s>`%s`\n\t%s\n", TOKEN_STRINGS[current->Type], get_value(current->ReadResult), debugInfo());
                                    panic(true);
                                }
                            }
                            assert(current->Type == TOKEN_OP_EQ);
                            NEXT_TOKEN();
                            char *variableValue = get_value(current->ReadResult);
                            printf("Current: <%s>%s\n", TOKEN_STRINGS[current->Type], get_value(current->ReadResult));
                            printf("\tVariable: %s, Type: %s, Override: %d\n", variableName, type, override);
                            if (type != NULL) {
                                if (!strcmp(type, "color")) {
                                    RGBA color = {0};
                                    assert(parseColor(variableValue, &color) != 1);

                                    Variable *var = calloc(1, sizeof(Variable));
                                    var->type = strdup(type);
                                    var->colorValue = color;

                                    ht_insert(ns->Properties, strdup(variableName), var);
                                } else {
                                    set_error_cat("[JINXST]");
                                    fprintf(stderr, "Unknown variable(%s) type: `%s`\n", variableName, type);
                                    fprintf(stderr, "\t%s\n", debugInfo());
                                    panic(true);
                                }
                            } else if (override) {
                                assert(false && "NOt implemented");
                            } else {
                                set_error_cat("[JINXST]");
                                fprintf(stderr, "Not type for variable yet variable override not specified. %s\n", debugInfo());
                                panic(true);
                            }
                            // free(variableName);
                            free(variableValue);
                            free(type); 
                        }
                        NEXT_TOKEN();
                        PANIC_IF_END();
                    }

                    GOTO_TOKEN_INDEX(scopeClose);
                    SAVE_TOKEN_STATE();
                } else {
                    REVERT_TOKEN_STATE();
                }
            } else {
                REVERT_TOKEN_STATE();
            }
            free(nameSpaceName);
        } else {
            set_error_cat("[JINXST]");
            fprintf(stderr, "Unexpected Token<%s>: `%s`\n", TOKEN_STRINGS[current->Type], get_value(current->ReadResult));
            panic(true);
        }
        NEXT_TOKEN();
    }

    globalStyles = getNamespace("global");
}
