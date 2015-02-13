#include <pebble_worker.h>

// This is an accelerometer sampling rate
// Available: 10, 25, 50 or 100 hz
#define SAMPLING_RATE ACCEL_SAMPLING_25HZ
// how many samples to approximate before calling our handler
#define NUM_SAMPLES 1
#define ACCEL_THRESHOLD 2500
#define mod(x) (x>0?x:-x)

static float my_sqrt(float x) {
	float a, p, e=.001, b;
	a = x;
	p = a*a;
	int nb = 0;
	while ((p-x >= e) && (nb++ < 50)) {
		b = (a + (x/a)) / 2;
		a = b;
		p = a*a;
	}
	return a;
}
static float sq(float x) {
	return x*x;
}

static int state = 0, count = 0;
static long start = 0;

static bool is_fall(float x, float y, float z, long time) {
	float norm = my_sqrt(x*x + y*y + z*z);
	long diff = start - time;

	if(state == 0) { // normal / waiting for fall
		if(norm < 3) { // free fall
			start = time;
			state = 1;
			count = 1;
		}
	} else if(state == 1) { // freefall
		if(diff > 1000) {
			state = 0;
		} else {
			if(norm < 3) // still falling
				count++;
			else
				state = 0; // not continuous -> not a fall
			if(count > 3) { // falling long enough
				count = 0;
				state = 2;
			}
		}
	} else if(state == 2) { // between freefall and impact
		if(diff > 2000) {
			state = 0; // too many time
		} else if(norm > 13) { // impact detected
			//start = time; // reset time
			state = 3;
		}
	} else if(state == 3) { // between impact and silence
		if(diff > 7000) {
			state = 0;
		} else if(9 < norm && norm < 11) { // stable
			//start = time; // reset time
			state = 4;
			count = 1;
		}
	} else if(state == 4) { // silence
		if(9 < norm && norm < 11) { // still stable
			count++;
			if(diff > 10000 || count > 50) { // silent for long enough
				state = 0;
				return true; // fall detected!
			}
		} else { // this was not a silence yet!
			start = time; // reset time
			state = 3;
		}
	}

	return false;
}

static void accel_handler(AccelData *data, uint32_t num_samples) {
	if(data->did_vibrate)
		return; // don't trust such data!

	if(is_fall(((float)data->x)/100, ((float)data->y)/100, ((float)data->z)/100, data->timestamp)) {
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
}

static void message_handler(uint16_t type, AppWorkerMessage *data) {
	if(type == 1) { // pause
		accel_data_service_unsubscribe();
	} else if(type == 2) { // unpause
		accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
		accel_service_set_sampling_rate(SAMPLING_RATE);
	} else { // unknown?
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Unknown message from app: %d", type);
	}
}

static void init() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Worker: init()");
	app_worker_message_subscribe(message_handler);
	accel_data_service_subscribe(NUM_SAMPLES, accel_handler);
	accel_service_set_sampling_rate(SAMPLING_RATE);
}

static void deinit() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Worker: deinit()");
	accel_data_service_unsubscribe();
	app_worker_message_unsubscribe();
}

int main(void) {
	init();
	worker_event_loop();
	deinit();
}
