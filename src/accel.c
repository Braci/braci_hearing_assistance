#include "accel.h"

static void accel_handler(AccelData *data, uint32_t num_samples) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %ld %d %d %d", data->did_vibrate, (long)data->timestamp,
			data->x, data->y, data->z);
	if(data->did_vibrate)
		return; // don't trust such data!

	if(mod(data->x) > ACCEL_THRESHOLD ||
			mod(data->y) > ACCEL_THRESHOLD ||
			mod(data->z) > ACCEL_THRESHOLD) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "USER FALLEN DOWN!");
		vibes_double_pulse();
	}
}

void accel_init() {
	accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
	accel_service_set_sampling_rate(SAMPLING_RATE);
}
void accel_deinit() {
	accel_data_service_unsubscribe();
}
