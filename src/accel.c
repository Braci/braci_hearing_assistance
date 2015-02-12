#include "accel.h"
#include "countdown.h"

void accel_user_falldown() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "USER FALLEN DOWN!");
	countdown_start();
	vibes_double_pulse();
}

static void worker_message_handler(uint16_t type, AppWorkerMessage *data) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Got message from worker, type = %d", type);
	if(type == 0)
		accel_user_falldown();
	else
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Unknown message from worker");
}

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
	app_worker_message_subscribe(worker_message_handler);
}
void accel_deinit() {
	app_worker_message_unsubscribe();
}
