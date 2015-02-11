#include <pebble_worker.h>

void accel_handler(AccelData *data, uint32_t num_samples) {
}

static void init() {
	accel_data_service_subscribe(10, accel_handler);
}

static void deinit() {
	accel_data_service_unsubscribe();
}

int main(void) {
	init();
	worker_event_loop();
	deinit();
}
