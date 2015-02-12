#include "accel.h"
#include "countdown.h"

void accel_start() {
	app_worker_launch();
}
void accel_stop() {
	app_worker_kill();
}
// if used after start()/stop(), uwait 100ms before calling!
bool accel_is_started() {
	return app_worker_is_running();
}

void accel_pause() {
}
void accel_unpause() {
}

void accel_init() {
}
void accel_deinit() {
}
