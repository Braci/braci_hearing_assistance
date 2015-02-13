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

long otime = 0;
float ox=0, oy=0, oz=0, onorm=0; // previous values
float v1=0, v2=0;
float oix=0, oiy=0, oiz=0;

static void init_detector() {
}
static bool is_fall(float x, float y, float z, long time) {
	float norm = 0;
	if(otime) {
		norm = my_sqrt(x*x + y*y + z*z);

		if(norm < 9.79) { // calculate integral
			float dtime = (time - otime) * 1000; // seconds since previous check

			v1 = v1 + (norm - 9.81) / dtime;
			
			float ix = oix + x/dtime;
		} else { // damp
			v1 = v1 * 0.95;
		}
	}
	otime = time;
	ox = x; oy = y; oz = z;
	onorm = norm;

	return norm < 0.46 && v1 > 1.72;
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
	init_detector();
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
