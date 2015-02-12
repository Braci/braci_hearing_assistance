#include "events.h"

uint32_t NOTIFY_ICONS[] = {
	RESOURCE_ID_IMAGE_FIREALARM,
	RESOURCE_ID_IMAGE_DOORBELL,
	RESOURCE_ID_IMAGE_BABYCRYING,
	RESOURCE_ID_IMAGE_TELEPHONE,
	RESOURCE_ID_IMAGE_CARHORN,
	RESOURCE_ID_IMAGE_THIEFALARM,
	RESOURCE_ID_IMAGE_ALARMCLOCK,
	RESOURCE_ID_IMAGE_WAKEUP,
	RESOURCE_ID_IMAGE_BEDTIME,
	RESOURCE_ID_IMAGE_SOS,
	RESOURCE_ID_IMAGE_EATTIME,
	RESOURCE_ID_IMAGE_NO,
	RESOURCE_ID_IMAGE_YES,
	RESOURCE_ID_IMAGE_CALLINGYOU,
	RESOURCE_ID_IMAGE_TRAFFIC,
	RESOURCE_ID_IMAGE_TRAIN,
	RESOURCE_ID_IMAGE_MICROWAVE,
	RESOURCE_ID_IMAGE_POLICE,
};
char NOTIFY_TEXTS[][30] = {
	"Fire Alarm",
	"Doorbell",
	"Baby Crying",
	"Telephone Ringing",
	"Car Horn",
	"Thief Alarm",
	"Alarm Clock",
	"Wake up!",
	"Bed Time",
	"I NEED YOUR HELP!",
	"Eat Time",
	"NO",
	"YES",
	"Calling You!",
	"Traffic Signal",
	"Train Horn",
	"Microwave",
	"Police",
};
const uint32_t VIBRATION_PATTERNS[][2] = { 
	{ 300, 100 }, // FIRE_ALARM 0
	{ 300, 500 }, // DOOR_BELL 1
	{ 700, 300 }, // BABY_CRYING 2
	{ 150, 800 }, // TELEPHONE 3
	{ 100, 100 }, // CAR_HORN 4
	{ 200, 700 }, // THIEF_ALARM 5
	{ 300, 300 }, // ALARM_CLOCK 6
	{ 500, 500 }, // WAKE_UP 7
	{ 200, 200 }, // BED_TIME 8
	{ 200, 100 }, // SOS 9
	{ 100, 300 }, // EAT_TIME 10
	{ 100, 200 }, // NO 11
	{ 600, 300 }, // YES 12
	{ 300, 300 }, // CALLING_YOU 13
	{ 300, 300 }, // TRAFFIC 14
	{ 300, 300 }, // TRAIN 15
	{ 300, 300 }, // Mircowave 16
	{ 300, 300 }, // Police 17
};
const uint16_t PAGING_INDICES[] = {
	7, 8, 9, 10, 11, 12, 13,
};
const uint16_t PAGING_INDICES_COUNT = ARRAY_LENGTH(PAGING_INDICES);
