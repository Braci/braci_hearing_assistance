#include "countdown.h"

static Window *wnd;
static TextLayer *tl;
static int count;
static char buf[4];

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	count--;
	snprintf(buf, sizeof(buf), "%02d", count);
	text_layer_set_text(tl, buf);

	if(count == 0) {
		tick_timer_service_unsubscribe();
		vibes_double_pulse();
		window_stack_remove(wnd, true);
	}
}

static void window_load(Window *wnd) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "wndload");
	Layer *root = window_get_root_layer(wnd);

	count = COUNTDOWN_SECONDS;
	snprintf(buf, sizeof(buf), "%02d", count);

	tl = text_layer_create(GRect(0,0,144,130));
	text_layer_set_background_color(tl, GColorBlack);
	text_layer_set_text_color(tl, GColorWhite);
	text_layer_set_font(tl, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	text_layer_set_text_alignment(tl, GTextAlignmentCenter);
	text_layer_set_text(tl, buf);
	layer_add_child(root, text_layer_get_layer(tl));

	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}
static void window_unload(Window *wnd) {
	tick_timer_service_unsubscribe();

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
