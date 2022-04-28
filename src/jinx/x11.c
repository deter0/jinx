#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "src/jinx/x11.h"

__attribute((noreturn)) static void PANIC(char *Message) {
	fprintf(stderr, "JINX Panicked! %s\n", Message);
	exit(1);
}

static void SetWindowClass(char *class, X11Window window) {
	XClassHint *xClass = XAllocClassHint();
	if (xClass == NULL)
		PANIC("Error allocating memory in `XAllocClassHint`");
	xClass->res_class = class;
	xClass->res_name = class;
	XSetClassHint(window.display, window.window, xClass);
	// XFree(xClass);
	// Maybe we free? ill rather take a memory leak over a seg-fault
}

static v2 GetDisplaySize(X11Window window) {
	int width = DisplayWidth(window.display, window.screen);
	int height = DisplayHeight(window.display, window.screen);
	return (v2){
		.x = width,
		.y = height
	};
}

X11Window createWindow(unsigned int width, unsigned int height) {
	Display *display = XOpenDisplay(NULL);
	if (display == NULL) {
		PANIC("Could not open display.");
	}
	int screen = DefaultScreen(display);

	Window win = XCreateSimpleWindow(
		display,
		RootWindow(display, screen),
		0, 0,
		width, height,
		1, BlackPixel(display, screen),
		BlackPixel(display, screen));
	XWindowAttributes attr = {0};
	X11Window xWindow = {
		.window = win,
		.display = display,
		.screen = screen,
		.dim = (dimensions){
			.size = (v2){
				.x = (int)width,
				.y = (int)height
			},
			.position = (v2) {
				.x = 0,
				.y = 0
			}
		},
		.attributes = &attr
	};
	Atom deleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(display, win, &deleteWindow, 1);

	SetWindowClass("jinx", xWindow);
	XStoreName(display, win, "jinx");
	v2 screen_size = GetDisplaySize(xWindow);

	XSelectInput(display, win, ButtonPressMask | ButtonReleaseMask | KeyPressMask | PointerMotionMask | StructureNotifyMask | ExposureMask);
	XMapWindow(display, win);
	
	xWindow.dim.position.x = screen_size.x / 2 - (xWindow.dim.size.x / 2);
	xWindow.dim.position.y = screen_size.y / 2 - (xWindow.dim.size.y / 2);

	XMoveWindow(display, win,
		xWindow.dim.position.x,
		xWindow.dim.position.y);
	XGetWindowAttributes(display, win, xWindow.attributes);
	
	return xWindow;
}
