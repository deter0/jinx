#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "reader.h"
#include "tokenizer.h"

void jinxst(char *jinxstSrc) {
    assert(jinxstSrc != NULL);
    read_result_pool *res = read_string(jinxstSrc);
    for (size_t i = 0; i < res->length; i++) {
        printf("[%s]\n", get_value(res->results[i]));
    }
    TokenPool *tokens = Tokenize(res);
    logTokenPool(stdout, tokens);
}
