#include <pebble_worker.h>

// Available: 10, 25, 50 or 100 hz
#define SAMPLING_RATE ACCEL_SAMPLING_10HZ
#define NUM_SAMPLES 10

void accel_handler(AccelData *data, uint32_t num_samples) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %ld %d %d %d", data->did_vibrate, (long)data->timestamp,
			data->x, data->y, data->z);
}

static void init() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Worker: init()");
	accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
	accel_service_set_sampling_rate(SAMPLING_RATE);
}

static void deinit() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Worker: deinit()");
	accel_data_service_unsubscribe();
}

int main(void) {
	init();
	worker_event_loop();
	deinit();
}
