#include <pebble_worker.h>

static void init() {
}

static void deinit() {
}

int main(void) {
	init();
	worker_event_loop();
	deinit();
}
