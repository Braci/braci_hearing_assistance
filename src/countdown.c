#include "countdown.h"

static Window *wnd;
static TextLayer *tl;
static char buf[4];

static void window_load(Window *wnd) {
	Layer *root = window_get_root_layer(wnd);

	tl = text_layer_create(GRect(0,0,144,130));
	text_layer_set_background_color(tl, GColorBlack);
	text_layer_set_text_color(tl, GColorWhite);
	text_layer_set_font(tl, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	text_layer_set_text(tl, "10");
	layer_add_child(root, text_layer_get_layer(tl));
}
static void window_unload(Window *wnd) {
	text_layer_destroy(tl);
}

void countdown_start() {
	window_stack_push(wnd, true);
}

void countdown_init() {
	wnd = window_create();
	window_set_fullscreen(wnd, false);
	window_set_window_handlers(wnd, (WindowHandlers) {
			.load = window_load,
			.unload = window_unload,
	});
}
void countdown_deinit() {
	window_destroy(wnd);
}
