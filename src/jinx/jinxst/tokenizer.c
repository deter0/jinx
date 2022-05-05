/*
	Author: deter0
	Originally written for love-lang (https://github.com/deter0/love)
	Modified for jinx
*/

#include "tokenizer.h"
#include "reader.h"

#include <string.h>
#include <ctype.h>
#include "log.h"

TokenPool *tokenPool(size_t initalAllocation) {
	TokenPool *tokenPool = (TokenPool*)chp(malloc(sizeof(TokenPool)));
	tokenPool->Allocated = initalAllocation;
	tokenPool->Tokens = (Token**)chp(calloc(1, sizeof(Token*) * tokenPool->Allocated));
	tokenPool->Length = 0;
	return tokenPool;
}
void addToken(TokenPool *pool, Token *to_add) {
	if (pool->Length + 1 >= pool->Allocated) {
		pool->Allocated = pool->Allocated * 1.5 + 10;
		pool->Tokens = (Token **)chp(realloc(pool->Tokens, sizeof(Token*) * pool->Allocated));
	}
	pool->Tokens[pool->Length] = to_add;
	pool->Length++;
}
char *indent = "";
void logTokenPool(FILE *fd, TokenPool *self) {
	fprintf(fd, "%s%sTokenPool%s {\n", indent, TERM_CYAN(), TERM_DEFAULT());
	for (size_t i = 0; i < self->Length; i++) {
		char *val = get_value(self->Tokens[i]->ReadResult);
		if (!strcmp(val, "\n")) {
			val = "<NEW LINE>";
		}
		fprintf(fd,
			"\t%sToken %s<%s>%s: %s\"%s\"%s,\n",
			indent, TERM_RED(),
			TOKEN_STRINGS[self->Tokens[i]->Type], TERM_DEFAULT(), TERM_GREEN(),
			get_value(self->Tokens[i]->ReadResult), TERM_DEFAULT()
		);
	}
	fprintf(fd, "%s}\n", indent);
}
void logTokenPool2D(FILE *fd, TokenPool2D *self) {
	fprintf(fd, "%sTokenPool2D%s [\n", TERM_CYAN(), TERM_DEFAULT());
	indent = "\t";
	for (size_t i = 0; i < self->Length; i++) {
		logTokenPool(fd, self->TokenPools[i]);
	}
	indent = "";
	fprintf(fd, "]\n");
}

TokenPool2D *tokenPool2D(size_t initalAllocation) {
	TokenPool2D *tokenPool2D = (TokenPool2D*)chp(malloc(sizeof(TokenPool2D)));
	tokenPool2D->Allocated = initalAllocation;
	tokenPool2D->TokenPools = (TokenPool**)chp(calloc(1, sizeof(TokenPool*) * tokenPool2D->Allocated));
	tokenPool2D->Length = 0;
	return tokenPool2D;
}
void addToken2D(TokenPool2D *pool, TokenPool *to_add) {
	if (pool->Length + 1 >= pool->Allocated) {
		pool->Allocated = pool->Allocated * 1.5 + 10;
		pool->TokenPools = (TokenPool **)chp(realloc(pool->TokenPools, sizeof(TokenPool*) * pool->Allocated));
	}
	pool->TokenPools[pool->Length] = to_add;
	pool->Length++;
}

// FIXME: This function has some problems LOL
TokenPool2D *tokenPoolSplit(TokenPool *pool, TokenType Delimeter) {
	TokenPool2D *result = tokenPool2D(pool->Allocated / 4);
	uint32_t Last = 0;
  if (pool->Length == 1) {
    TokenPool *cpy = (TokenPool*)chp(malloc(sizeof(TokenPool)));
    memcpy(cpy, pool, sizeof(TokenPool));
    addToken2D(result, cpy);
    return result;
  }
	for (size_t i = 0; i < pool->Length; i++) {
		if (pool->Tokens[i]->Type == Delimeter || i == pool->Length-1) {
			TokenPool *split = tokenPool(i - Last);
			for (size_t j = Last; j < i; j++) {
				addToken(split, pool->Tokens[j]);
			}
			addToken2D(result, split);
			Last = i + 1;
		}
	}
	return result;
}

static char *ltrim(char *s) {
    while(isspace(*s)) s++;
    return s;
}

static char *rtrim(char *s) {
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

static char *trim(char *s) {
    return rtrim(ltrim(s)); 
}

TokenPool *Tokenize(read_result_pool *parsed) {
	TokenPool *tokenPool = (TokenPool*)chp(calloc(1, sizeof(TokenPool)));
	tokenPool->Allocated = 200;
	tokenPool->Tokens = (Token**)chp(calloc(1, sizeof(Token) * tokenPool->Allocated));
	tokenPool->Length = 0;
	
	for (size_t i = 0; i < parsed->length; ++i) {
		char *val = get_value(parsed->results[i]);
		Token *tkn = (Token*)chp(malloc(sizeof(Token)));
		tkn->Type = TOKEN_UNDEFINED;
		if (!strcmp("return", val)) {
			tkn->Type = TOKEN_KEYWORD_RETURN;
		} else if (!strcmp("int", val)) {
			tkn->Type = TOKEN_TYPE_INT;
		} else if (!strcmp("void", val)) {
			tkn->Type = TOKEN_TYPE_VOID;
		} else if (val[0] == '"' && val[strlen(val)-1] == '"') {
			tkn->Type = TOKEN_STRING_LITERAL;
		} else if (val[0] == '\'' && val[strlen(val)-1] == '\'') {
			tkn->Type = TOKEN_CHAR;
		} else if (!strcmp(";", val)) {
			tkn->Type = TOKEN_SEMICOLON;
		} else if (!strcmp("{", val)) {
			tkn->Type = TOKEN_OPEN_SCOPE;
		} else if (!strcmp("}", val)) {
			tkn->Type = TOKEN_CLOSE_SCOPE;
		} else if (!strcmp("(", val)) {
			tkn->Type = TOKEN_OPEN_PAREN;
		} else if (!strcmp(")", val)) {
			tkn->Type = TOKEN_CLOSE_PAREN;
		} else if (!strcmp("=", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp(next, "-")) {
					tkn->Type = TOKEN_COMPARE;
					parsed->results[i]->length++;
					i += 1;
				} else {
					tkn->Type = TOKEN_OP_EQ;
				}
			}
			tkn->Type = TOKEN_OP_EQ;
		} else if (!strcmp("-", val)) {
			tkn->Type = TOKEN_OP_SUB;
		} else if (!strcmp("+", val)) {
			tkn->Type = TOKEN_OP_ADD;
		} else if (!strcmp("*", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp("/", next)) {
					tkn->Type = TOKEN_CLOSE_COMMENT;
					parsed->results[i]->length++;
					i++;
				}
			} else {
				tkn->Type = TOKEN_OP_MUL;
			}
		} else if (!strcmp("/", val)) {
			if ((i + 1) <= parsed->length) {
				char *next = get_value(parsed->results[i + 1]);
				if (!strcmp("*", next)) {
					tkn->Type = TOKEN_COMMENT;
					parsed->results[i]->length++;
					i++;
				}
			} else {
				tkn->Type = TOKEN_OP_DIV;
			}
		} else if (!strcmp(",", val)) {
			tkn->Type = TOKEN_COMMA;
		} else if (!strcmp(">", val)) {
			tkn->Type = TOKEN_OP_MORE_THAN;
		} else if (!strcmp("<", val)) {
			tkn->Type = TOKEN_OP_LESS_THAN;
		} else if (!strcmp("$", val)) {
			tkn->Type = TOKEN_VARIABLE;
		} else if (!strcmp("!", val)) {
			tkn->Type = TOKEN_OVERRIDE;
		} else {
			if (strcmp(trim(val), "")) {
				tkn->Type = TOKEN_WORD;
			}
		}
		tkn->ReadResult = parsed->results[i];
		if (tkn->Type != TOKEN_UNDEFINED) {
			if (tokenPool->Length > 0 && tokenPool->Tokens[tokenPool->Length - 1]->Type == TOKEN_COMMENT && tkn->Type != TOKEN_CLOSE_COMMENT) {
				free(tkn);
				continue;
			} else if (tokenPool->Length > 0 && tokenPool->Tokens[tokenPool->Length - 1]->Type == TOKEN_COMMENT && tkn->Type == TOKEN_CLOSE_COMMENT) {
				free(tokenPool->Tokens[tokenPool->Length  - 1]);
				free(tkn);
				tokenPool->Length--;
				continue;
			}
#ifdef DEBUG
			printf("Added token: <%s> : '%s'\n", TOKEN_STRINGS[(int)tkn->Type], get_value(parsed->results[i]));
#endif
			addToken(tokenPool, tkn);
		} else {
			free(tkn);
		}
	}
	Token *eof = (Token*)chp(malloc(sizeof(Token)));
	eof->ReadResult = (read_result*)chp(malloc(sizeof(read_result)));
	eof->ReadResult->is_end = true;
	eof->ReadResult->length = 1;
	eof->ReadResult->ptr_start = parsed->src + strlen(parsed->src-1);
	eof->Type = TOKEN_EOF;
	addToken(tokenPool, eof);
	return tokenPool;
}
