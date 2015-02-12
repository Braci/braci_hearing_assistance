#include "splash.h"
#include "events.h"

static Window *wnd;
static TextLayer *tl;

static int event_to_send;

static void timer_callback(void *data) {
	window_stack_remove(wnd, true);
}

static void window_load(Window *wnd) {
	Layer *root = window_get_root_layer(wnd);

	tl = text_layer_create(GRect(0,0,144,60));
	text_layer_set_background_color(tl, GColorBlack);
	text_layer_set_text_color(tl, GColorWhite);
	text_layer_set_font(tl, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(tl, GTextAlignmentCenter);
	text_layer_set_text(tl, "Sent");
	layer_add_child(root, text_layer_get_layer(tl));

	app_timer_register(SPLASH_TIMEOUT, timer_callback, NULL);
}
static void window_unload(Window *wnd) {
	text_layer_destroy(tl);
}

void splash_send(int event) {
	event_to_send = event;
	window_stack_push(wnd, true);

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending event %d", event);
	// Now send the message to phone
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_uint16(iter, 0, event);
	app_message_outbox_send();
}

void splash_init() {
	wnd = window_create();
	window_set_background_color(wnd, GColorBlack);
	window_set_window_handlers(wnd, (WindowHandlers) {
			.load = window_load,
			.unload = window_unload,
	});
}
void splash_deinit() {
	window_destroy(wnd);
}
