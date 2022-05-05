#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

// https://stackoverflow.com/a/38175137
struct MwmHints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
};
enum {
    MWM_HINTS_FUNCTIONS = (1L << 0),
    MWM_HINTS_DECORATIONS =  (1L << 1),

    MWM_FUNC_ALL = (1L << 0),
    MWM_FUNC_RESIZE = (1L << 1),
    MWM_FUNC_MOVE = (1L << 2),
    MWM_FUNC_MINIMIZE = (1L << 3),
    MWM_FUNC_MAXIMIZE = (1L << 4),
    MWM_FUNC_CLOSE = (1L << 5)
};

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

Display *display;

X11Window createWindow(unsigned int width, unsigned int height) {
	if (display == NULL)
		display = XOpenDisplay(NULL);
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

/*
X11Window createTooltip(unsigned int width, unsigned int height) {
	if (display == NULL) {
		PANIC("NO display open");
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

	SetWindowClass("jinxTooltip", xWindow);
	XStoreName(display, win, "jinxTooltip");
	v2 screen_size = GetDisplaySize(xWindow);

	Atom data[4];
	Atom netWMState = XInternAtom(display, "_NET_WM_STATE", false);
	data[0] = XInternAtom(display, "_NET_WM_STATE_MODAL", false);
	data[1] = XInternAtom(display, "_NET_WM_STATE_ABOVE", false);
	XChangeProperty(display, win, netWMState, XA_ATOM, 32,
		PropModeAppend, (unsigned char*)data, 2);

	Atom netWMType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", false);
	data[0] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_TOOLTIP", false);
	data[1] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", false);
	XChangeProperty(display, win, netWMType, XA_ATOM, 32,
		PropModeReplace, (unsigned char*)data, 1);

	struct MwmHints hints0;
	Atom wm = XInternAtom(display, "_MOTIF_WM_HINTS", False);
	hints0.functions = MWM_FUNC_CLOSE;
	hints0.flags = MWM_HINTS_DECORATIONS;
	hints0.decorations = 0;
	XChangeProperty(display, win, wm, XA_ATOM, 32, PropModeReplace, (unsigned char*)&hints0, 5);

	// long hints[5] = {2, 0, 0, 0, 0};
	// Atom motifHints = XInternAtom(display, "_MOTIF_WM_HINTS", false);
	// XChangeProperty(display, win, motifHints, motifHints, 32, PropModeAppend,
	// 				(unsigned char *)&hints, 5);

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
*/
