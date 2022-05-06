/*
	Author: deter0
	Originally written for love-lang (https://github.com/deter0/love)
*/

#pragma once
#include "reader.h"
#include <stdio.h>

typedef enum TokenType {
	TOKEN_UNDEFINED, //
	TOKEN_NEW_LINE, //
	TOKEN_SEMICOLON, //
	TOKEN_KEYWORD_RETURN, //
	TOKEN_STRING_LITERAL, //
	TOKEN_CHAR, //
	TOKEN_TYPE_VOID, //
	TOKEN_TYPE_STRING,
	TOKEN_TYPE_NUMBER, //
	TOKEN_TYPE_COLOR,
	TOKEN_OPEN_SCOPE, //
	TOKEN_CLOSE_SCOPE, //
	TOKEN_OPEN_PAREN, //
	TOKEN_CLOSE_PAREN, //
	TOKEN_OP_EQ, //
	TOKEN_OP_ADD, //
	TOKEN_OP_SUB, //
	TOKEN_OP_DIV, //
	TOKEN_OP_MUL, //
	TOKEN_WORD, //
	TOKEN_COMMA, //
	TOKEN_OP_LESS_THAN,
	TOKEN_OP_MORE_THAN,
	TOKEN_COMMENT,
    TOKEN_CLOSE_COMMENT,
	TOKEN_EOF,
	TOKEN_COMPARE, //
    TOKEN_VARIABLE,
    TOKEN_OVERRIDE,
    TOKEN_INHERIT,
	TOKEN_TYPE_ASSIGN,
} TokenType;

const char *TOKEN_STRINGS[100] = {
	"TOKEN_UNDEFINED", //
	"TOKEN_NEW_LINE", //
	"TOKEN_SEMICOLON", //
	"TOKEN_KEYWORD_RETURN", //
	"TOKEN_STRING_LITERAL", //
	"TOKEN_CHAR", //
	"TOKEN_TYPE_VOID", //
	"TOKEN_TYPE_STRING",
	"TOKEN_TYPE_NUMBER", //
	"TOKEN_TYPE_COLOR",
	"TOKEN_OPEN_SCOPE", //
	"TOKEN_CLOSE_SCOPE", //
	"TOKEN_OPEN_PAREN", //
	"TOKEN_CLOSE_PAREN", //
	"TOKEN_OP_EQ", //
	"TOKEN_OP_ADD", //
	"TOKEN_OP_SUB", //
	"TOKEN_OP_DIV", //
	"TOKEN_OP_MUL", //
	"TOKEN_WORD", //
	"TOKEN_COMMA", //
	"TOKEN_OP_LESS_THAN",
	"TOKEN_OP_MORE_THAN",
	"TOKEN_COMMENT",
    "TOKEN_CLOSE_COMMENT",
	"TOKEN_EOF",
	"TOKEN_COMPARE", //
    "TOKEN_VARIABLE",
    "TOKEN_OVERRIDE",
    "TOKEN_INHERIT",
	"TOKEN_TYPE_ASSIGN",
};

typedef struct Token {
	TokenType Type;
	read_result *ReadResult;
} Token;

typedef struct TokenPool {
	size_t Allocated;
	size_t Length;
	Token **Tokens;
	char *src;
} TokenPool;

typedef struct TokenPool2D {
	size_t Allocated;
	size_t Length;
	TokenPool **TokenPools;
} TokenPool2D;

TokenPool *tokenPool(size_t initalAllocation);
void addToken(TokenPool *pool, Token *to_add);
void logTokenPool(FILE *fd, TokenPool *self);

TokenPool2D *tokenPool2D(size_t initalAllocation);
void addToken2D(TokenPool2D *pool, TokenPool *to_add);
void logTokenPool2D(FILE *fd, TokenPool2D *self);

TokenPool2D *tokenPoolSplit(TokenPool *pool, TokenType Delimeter);

TokenPool *Tokenize(read_result_pool *parsed, char *src);
char *trim(char *s);
