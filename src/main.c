#include <pebble.h>
#include "accel.h"
#include "countdown.h"
#include "paging.h"
#include "events.h"

static Window *window;

static BitmapLayer *image_layer;
static TextLayer *temperature_layer;

static Layer *window_layer;

static GBitmap *image;

static AppSync sync;
static uint8_t sync_buffer[32];

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

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Got new tuple: %d", new_tuple->value->uint8);

	unsigned int i = 0;
	if(new_tuple->value->uint8 == 100)
	{
		return;
	}
	if(new_tuple->value->uint8 == 101)
	{
		vibes_cancel();
		vibes_short_pulse();
		// window_unload();
		const bool animated = true;
		window_stack_remove(window, animated);
		return;
	}

	for (i = 0; i < ARRAY_LENGTH(segments); i++)
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
}

void any_button_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	Tuplet symbol_tuple = TupletInteger(0, 1);
	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &symbol_tuple);
	dict_write_end(iter);
	app_message_outbox_send();

	vibes_cancel();
	vibes_short_pulse();

//	window_unload();
	const bool animated = true;
	window_stack_remove(window, animated);
}
void up_button_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
	if(app_worker_is_running()) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopping worker");
		app_worker_kill();
	} else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Launching worker");
		app_worker_launch();
	}
}
void select_button_single_click_handler(ClickRecognizerRef recognizer, void *ctx) {
	paging_open();
}

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_BACK, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_button_single_click_handler);
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

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Launch reason: %d", launch_reason());

	// Initializing app_sync:
	Tuplet initial_values[] = {
		TupletInteger(NOTIFY_ICON_KEY, (uint8_t) 100),
	};
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
		sync_tuple_changed_callback, sync_error_callback, NULL);

	const int inbound_size = 64;
	const int outbound_size = 16;
	app_message_open(inbound_size, outbound_size);

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

	const bool animated = true;
	window_stack_push(window, animated);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "init: done");
}
static void deinit(void) {
	app_sync_deinit(&sync);
	app_message_deregister_callbacks();

	paging_deinit();
	accel_deinit();
	countdown_deinit();

	if(window){
		window_destroy(window);
	}
}

int main(void) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main: before init()");
	init();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main: before app_event_loop()");

	app_event_loop();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "main: before deinit()");
	deinit();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "main: done");
}
