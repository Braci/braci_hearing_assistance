#include "accel.h"
#include "countdown.h"

static void accel_handler(AccelData *data, uint32_t num_samples) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %ld %d %d %d", data->did_vibrate, (long)data->timestamp,
			data->x, data->y, data->z);
	if(data->did_vibrate)
		return; // don't trust such data!

	if(mod(data->x) > ACCEL_THRESHOLD ||
			mod(data->y) > ACCEL_THRESHOLD ||
			mod(data->z) > ACCEL_THRESHOLD) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "USER FALLEN DOWN!");
		countdown_start();
		vibes_double_pulse();
	}
}

static bool is_started;
void accel_start() {
	accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
	accel_service_set_sampling_rate(SAMPLING_RATE);
	is_started = true;
}
void accel_stop() {
	is_started = false;
	accel_data_service_unsubscribe();
}

void accel_pause() {
	if(is_started)
		accel_data_service_unsubscribe(); // but don't toggle is_started
}
void accel_unpause() {
	if(is_started)
		accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
}

void accel_init() {
	accel_start();
}
void accel_deinit() {
	accel_stop();
}
