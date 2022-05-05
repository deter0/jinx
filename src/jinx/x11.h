#pragma once
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cairo/cairo.h>
#include <stdint.h>

typedef struct v2 {
	int x;
	int y;
} v2;

typedef struct dimensions {
	v2 position;
	v2 size;
} dimensions;

typedef struct X11Window {
	Display *display;
	Window window;
	int screen;
	dimensions dim;
	XWindowAttributes *attributes;
} X11Window;

X11Window createWindow(unsigned int width, unsigned int height);
// X11Window createTooltip(unsigned int width, unsigned int height);
