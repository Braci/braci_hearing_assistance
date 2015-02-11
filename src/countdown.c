#include "countdown.h"

static Window *wnd;
static TextLayer *tl_head, *tl_count, *tl_bottom;
static int count;
static char buf[4];

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	bool minus = false;
	if(count > 0) {
		count--;
		snprintf(buf, sizeof(buf), "%d", count);
		text_layer_set_text(tl_count, buf);
	} else {
		count++;
		minus = true;
	}

	if(count == 0) {
		if(minus) {
			tick_timer_service_unsubscribe();
			window_stack_remove(wnd, true);
		} else { // countdown done, do action and wait for delay
			vibes_double_pulse();
			// TODO: action here

			count = -COUNTDOWN_MESSAGE_DELAY;
			text_layer_set_text(tl_head, "Sending SOS now");
			text_layer_set_text(tl_count, "");
			text_layer_set_text(tl_bottom, "");
		}
	}
}

static void button_handler(ClickRecognizerRef rec, void *ctx) {
	count = -COUNTDOWN_MESSAGE_DELAY;
	text_layer_set_text(tl_head, "Cancelled");
	text_layer_set_text(tl_count, "");
	text_layer_set_text(tl_bottom, "");
}

static void click_config_provider(void *ctx) {
	for(int btn = 0; btn<4; btn++)
		window_single_click_subscribe(btn, button_handler);
}

static void window_load(Window *wnd) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "wndload");
	Layer *root = window_get_root_layer(wnd);

	count = COUNTDOWN_SECONDS;
	snprintf(buf, sizeof(buf), "%02d", count);

	tl_head = text_layer_create(GRect(0,0,144,60));
	text_layer_set_background_color(tl_head, GColorBlack);
	text_layer_set_text_color(tl_head, GColorWhite);
	text_layer_set_font(tl_head, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(tl_head, GTextAlignmentCenter);
	text_layer_set_text(tl_head, "Will send SOS in");
	layer_add_child(root, text_layer_get_layer(tl_head));

	tl_count = text_layer_create(GRect(0,60,144,60));
	text_layer_set_background_color(tl_count, GColorBlack);
	text_layer_set_text_color(tl_count, GColorWhite);
	text_layer_set_font(tl_count, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	text_layer_set_text_alignment(tl_count, GTextAlignmentCenter);
	text_layer_set_text(tl_count, buf);
	layer_add_child(root, text_layer_get_layer(tl_count));

	tl_bottom = text_layer_create(GRect(0,120,144,40));
	text_layer_set_background_color(tl_bottom, GColorBlack);
	text_layer_set_text_color(tl_bottom, GColorWhite);
	text_layer_set_font(tl_bottom, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	text_layer_set_text_alignment(tl_bottom, GTextAlignmentCenter);
	text_layer_set_text(tl_bottom, "seconds");
	layer_add_child(root, text_layer_get_layer(tl_bottom));

	window_set_click_config_provider(wnd, click_config_provider);

	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}
static void window_unload(Window *wnd) {
	tick_timer_service_unsubscribe();

	text_layer_destroy(tl_head);
	text_layer_destroy(tl_count);
	text_layer_destroy(tl_bottom);
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
