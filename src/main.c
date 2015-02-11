/*

   Demonstrate how to display a two color, black and white bitmap
   image with no transparency.

   The original source image is from:

      <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>

   The source image was converted from an SVG into a RGB bitmap using
   Inkscape. It has no transparency and uses only black and white as
   colors.

 */

// This is an accelerometer sampling rate
// Available: 10, 25, 50 or 100 hz
#define SAMPLING_RATE ACCEL_SAMPLING_10HZ
#define NUM_SAMPLES 10

#include "pebble.h"

static Window *window;

static BitmapLayer *image_layer;
static TextLayer *temperature_layer;

static Layer *window_layer;

static GBitmap *image;

static AppSync sync;
static uint8_t sync_buffer[32];

static uint32_t NOTIFY_ICONS[] = {
	RESOURCE_ID_IMAGE_FIREALARM,
	RESOURCE_ID_IMAGE_DOORBELL,
	RESOURCE_ID_IMAGE_BABYCRYING,
	RESOURCE_ID_IMAGE_TELEPHONE,
	RESOURCE_ID_IMAGE_CARHORN,
	RESOURCE_ID_IMAGE_THIEFALARM,
	RESOURCE_ID_IMAGE_ALARMCLOCK,
	RESOURCE_ID_IMAGE_WAKEUP,
	RESOURCE_ID_IMAGE_BEDTIME,
	RESOURCE_ID_IMAGE_SOS,
	RESOURCE_ID_IMAGE_EATTIME,
	RESOURCE_ID_IMAGE_NO,
	RESOURCE_ID_IMAGE_YES,
	RESOURCE_ID_IMAGE_CALLINGYOU,
	RESOURCE_ID_IMAGE_TRAFFIC,
	RESOURCE_ID_IMAGE_TRAIN,
	RESOURCE_ID_IMAGE_MICROWAVE,
	RESOURCE_ID_IMAGE_POLICE,
};

static const uint32_t VIBRATION_PATTERNS[18][2] = { 
	{ 300, 100 }, // FIRE_ALARM 0
	{ 300, 500 }, // DOOR_BELL 1
	{ 700, 300 }, // BABY_CRYING 2
	{ 150, 800 }, // TELEPHONE 3
	{ 100, 100 }, // CAR_HORN 4
	{ 200, 700 }, // THIEF_ALARM 5
	{ 300, 300 }, // ALARM_CLOCK 6
	{ 500, 500 }, // WAKE_UP 7
	{ 200, 200 }, // BED_TIME 8
	{ 200, 100 }, // SOS 9
	{ 100, 300 }, // EAT_TIME 10
	{ 100, 200 }, // NO 11
	{ 600, 300 }, // YES 12
	{ 300, 300 }, // CALLING_YOU 13
	{ 300, 300 }, // TRAFFIC 14
	{ 300, 300 }, // TRAIN 15
	{ 300, 300 }, // Mircowave 16
	{ 300, 300 }, // Police 17
};

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

static char NOTIFY_TEXTS[][30] = {
	"Fire Alarm",
	"Doorbell",
	"Baby Crying",
	"Telephone Ringing",
	"Car Horn",
	"Thief Alarm",
	"Alarm Clock",
	"Wake up!",
	"Bed Time",
	"I NEED YOUR HELP!",
	"Eat Time",
	"NO",
	"YES",
	"Calling You!",
	"Traffic Signal",
	"Train Horn",
	"Microwave",
	"Police",
};

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

void accel_handler(AccelData *data, uint32_t num_samples) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %ld %d %d %d", data->did_vibrate, (long)data->timestamp,
			data->x, data->y, data->z);
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

void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_BACK, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, any_button_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, any_button_single_click_handler);

	window_single_click_subscribe(BUTTON_ID_UP, up_button_single_click_handler);
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

	accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
	accel_service_set_sampling_rate(SAMPLING_RATE);

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

	accel_data_service_unsubscribe();

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
