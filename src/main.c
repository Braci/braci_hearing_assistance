#include <pebble.h>
#include "accel.h"
#include "countdown.h"
#include "paging.h"
#include "events.h"
#include "splash.h"

static Window *window;

static BitmapLayer *image_layer;
static TextLayer *temperature_layer;

static Layer *window_layer;

static GBitmap *image;

static AppTimer *timer = NULL;

static uint32_t segments[] = {
	200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200,
	200, 200, 200, 200, 200, 200, 
	200, 200, 200, 200, 200, 200
};

VibePattern pat;

enum NotifyKey {
	NOTIFY_ICON_KEY = 0x0,         // TUPLE_INT
	NOTIFY_TEXT_KEY = 0x1,  // TUPLE_CSTRING
};

static void inbox_received_callback(DictionaryIterator *iter, void *ctx) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Inbox received!");

	Tuple *new_tuple = dict_find(iter, NOTIFY_ICON_KEY);

	if(timer)
		app_timer_cancel(timer); // cancel timeout, as we got message and will show it now

	if(new_tuple->value->uint8 == 101)
	{
		vibes_cancel();
		vibes_short_pulse();
		const bool animated = true;
		window_stack_remove(window, animated);
		return;
	}

	for (unsigned int i = 0; i < ARRAY_LENGTH(segments); i++)
	{
		segments[i] = VIBRATION_PATTERNS[new_tuple->value->uint8][i % 2];
	}

	pat.durations = segments;
	pat.num_segments = ARRAY_LENGTH(segments);


	light_enable(true);
	vibes_enqueue_custom_pattern(pat);
	if (image) {
		gbitmap_destroy(image);
	}
	image = gbitmap_create_with_resource(NOTIFY_ICONS[new_tuple->value->uint8]);

	// The bitmap layer holds the image for display
	bitmap_layer_set_bitmap(image_layer, image);
	text_layer_set_text(temperature_layer, NOTIFY_TEXTS[new_tuple->value->uint8]);
	if(timer) {
		window_stack_push(window, true);
		timer = NULL;
	}
}

static void timer_callback(void *data) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer fired");
	window_stack_push(window, true);
	timer = NULL;
}

static void any_button_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	if (iter) {
		dict_write_uint16(iter, 0, 1000);
		app_message_outbox_send();

		vibes_cancel();
		vibes_short_pulse();
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Couldn't initialize outbox sending");
	}

	const bool animated = true;
	window_stack_remove(window, animated);
}

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_BACK, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, any_button_single_click_handler);
}

static void window_load(Window *window) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "window_load: hello");
	window_layer = window_get_root_layer(window);
	// This needs to be deinited on app exit which is when the event loop ends
	image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EMPTY);

	// The bitmap layer holds the image for display
	image_layer = bitmap_layer_create(GRect(0, 0, 144, 130));
	bitmap_layer_set_bitmap(image_layer, image);
	bitmap_layer_set_alignment(image_layer, GAlignCenter);
	bitmap_layer_set_compositing_mode(image_layer, GCompOpAssign);
	layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));

	temperature_layer = text_layer_create(GRect(0, 130, 144, 38));
	text_layer_set_text_color(temperature_layer, GColorWhite);
	text_layer_set_background_color(temperature_layer, GColorBlack);
	text_layer_set_font(temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
	text_layer_set_text(temperature_layer, "");
	layer_add_child(window_layer, text_layer_get_layer(temperature_layer));

	window_set_click_config_provider(window, click_config_provider);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "window_load: done");
}
static void window_unload() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "window_unload: hello");

	if (image) {
		gbitmap_destroy(image);
	}

	text_layer_destroy(temperature_layer);
	bitmap_layer_destroy(image_layer);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "window_unload: done");
}

static void init() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "init: hello");

	const int inbound_size = 64;
	const int outbound_size = 16;
	app_message_open(inbound_size, outbound_size);
	app_message_register_inbox_received(inbox_received_callback);

	splash_init();
	countdown_init();
	accel_init();
	paging_init();

	window = window_create();
	//  window_set_background_color(window, GColorBlack);
	window_set_fullscreen(window, true);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload
	});

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Launch reason: %d", launch_reason());

	switch(launch_reason()) {
		case APP_LAUNCH_SYSTEM:
		case APP_LAUNCH_USER:
		case APP_LAUNCH_QUICK_LAUNCH:
			paging_open();
			break;

		case APP_LAUNCH_PHONE:
			//timer = app_timer_register(1000, timer_callback, NULL);
			window_stack_push(window, true);
			// either incoming message or launched from debug console
			break;

		case APP_LAUNCH_WORKER:
			accel_user_falldown();
			break;

		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Unknown launch reason!");
			break;
	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "init: done");
}
static void deinit(void) {
	app_message_deregister_callbacks();

	paging_deinit();
	accel_deinit();
	countdown_deinit();
	splash_deinit();

	if(window){
		window_destroy(window);
	}
}

int main(void) {
	init();

	app_event_loop();
	/*
	// This trick seems to break something in the system... so disabling it for now
	while(timer) { // launch not finished yet! so if evt loop stopped (because of no windows are active), relaunch it
		psleep(10);
		app_event_loop();
	}
	*/

	deinit();
}
