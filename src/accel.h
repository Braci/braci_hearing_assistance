#pragma once
#include <pebble.h>

// This is an accelerometer sampling rate
// Available: 10, 25, 50 or 100 hz
#define SAMPLING_RATE ACCEL_SAMPLING_10HZ
#define NUM_SAMPLES 2
#define ACCEL_THRESHOLD 1800
#define mod(x) (x>0?x:-x)

void accel_init();
void accel_deinit();
void accel_start();
void accel_stop();
void accel_pause();
void accel_unpause();
