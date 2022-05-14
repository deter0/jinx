/*
	Author: deter0
	Originally written for love-lang (https://github.com/deter0/love)
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

inline const char *TERM_RED();
inline const char *TERM_UNDERLINE(); 
inline const char *TERM_GREEN(); 
inline const char *TERM_BLUE();
inline const char *TERM_CYAN();
inline const char *TERM_DEFAULT();

void apply_color(FILE *fs, char *color);
void set_error_cat(char *cat);
void panic(bool has_context);
void *chp(void *ptr);
void l_assert(int condition, char *context);
