#include "countdown.h"

static Window *wnd;

static void window_load(Window *wnd) {
	//
}
static void window_unload(Window *wnd) {
}

void countdown_start() {
	window_stack_push(window, true);
}

void countdown_init() {
	win = window_create();
	window_set_window_handlers(wnd, (WindowHandlers) {
			.load = window_load,
			.unload = window_unload,
	});
}
void countdown_deinit() {
	window_destroy(wnd);
}
