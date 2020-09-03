#pragma once

#include "interfaces.h"

typedef struct aileron
{
	interface_t functions;

	float current_angle;
	float dest_angle;
	int state;

	float max_top_angle;
	float max_low_angle;
	float speed;
	int pos; //left = -1; right = 1
} aileron_t;

void* aileron_init(int pos);
void aileron_release(void* aileron);