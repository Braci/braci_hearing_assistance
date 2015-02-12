#include "paging.h"

static Window *wnd;
static MenuLayer *menu;

static void window_load(Window *wnd) {
}
static void window_unload(Window *wnd) {
}

void paging_open() {
	window_stack_push(wnd, true);
}

void paging_init() {
	wnd = window_create();
	window_set_window_handlers(wnd, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});
}
void paging_deinit() {
	window_destroy(wnd);
}
