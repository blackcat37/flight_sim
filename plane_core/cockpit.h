#pragma once

#include "interfaces.h"

typedef struct daytime
{
	int hour;
	int min;
	int sec;
	int msec;
} daytime_t;

typedef struct cockpit
{
	interface_t functions;

	daytime_t time;
	float tas;
	float heading;
	float bank;
	float aileron_left;
	float aileron_right;
	float load;
	long start_flight;

	void** elements;
	int elements_count;
} cockpit_t;

void* cockpit_init();
void cocpit_realese(void* cockpit);

void add_dependency(void* cockpit, void* elem);
