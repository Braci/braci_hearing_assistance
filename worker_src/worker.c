#include <pebble_worker.h>

// This is an accelerometer sampling rate
// Available: 10, 25, 50 or 100 hz
#define SAMPLING_RATE ACCEL_SAMPLING_10HZ
#define NUM_SAMPLES 2
#define ACCEL_THRESHOLD 1800
#define mod(x) (x>0?x:-x)

void accel_handler(AccelData *data, uint32_t num_samples) {
	if(data->did_vibrate)
		return; // don't trust such data!

	if(mod(data->x) > ACCEL_THRESHOLD ||
			mod(data->y) > ACCEL_THRESHOLD ||
			mod(data->z) > ACCEL_THRESHOLD) {
		// Send a message in case the app is already started...
		AppWorkerMessage msg = {
			.data0 = (uint16_t)data->x,
			.data1 = (uint16_t)data->y,
			.data2 = (uint16_t)data->z,
		};
		app_worker_send_message(0, &msg);
		// ...or start the app, if it was not running
		worker_launch_app();
	}
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
