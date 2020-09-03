#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "interface.h"
#include "aileron.h"

typedef struct aileron
{
	unit_t actions;

	float current_angle;
	
	int last_time;
	
	int direction; //1 - forward, 0 - stop, -1 - reverse

	float max_top_angle;
	float max_low_angle;
	float speed;
	int pos; //left = -1; right = 1
} aileron_t;

const char* aileron_get_module_name()
{
	return "aileron";
}

void aileron_get_float_param(void* aileron, const char* param_name, float* out)
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "angle") == 0)
	{
		*out = ((aileron_t*)ptr)->current_angle;
	}
}

void aileron_get_int_param(void* aileron, const char* param_name, int* out)
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "direction") == 0)
	{
		*out = ((aileron_t*)ptr)->direction;
	}
	else if (strcmp(param_name, "position") == 0)
	{
		*out = ((aileron_t*)ptr)->pos;
	}
}

void aileron_set_float_param(void* aileron, const char* param_name, float in)
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "angle") == 0)
	{
		((aileron_t*)ptr)->current_angle = in;
	}
}

void aileron_set_int_param(void* aileron, const char* param_name, int in)
{
	return;
}

void aileron_display(void* aileron, int update)
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	if (((aileron_t*)ptr)->pos == -1)
		printf("Aileron:\tleft: %02.1f ", ((aileron_t*)aileron)->current_angle);
	else
		printf("\tright: %02.1f ", ((aileron_t*)aileron)->current_angle);
}

void aileron_activate(void* aileron, int time, int direction) //1 - forward, 0 - stop, -1 - reverse
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	((aileron_t*)ptr)->direction = direction;
	//((aileron_t*)ptr)->last_time = time;
}

void aileron_action(void* aileron, int time)
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	float delta;
	float shift;

	delta = (time - ((aileron_t*)ptr)->last_time) / 1000.0f;
	shift = ((aileron_t*)ptr)->speed * delta * ((aileron_t*)ptr)->direction;

	((aileron_t*)ptr)->current_angle += shift;
	((aileron_t*)ptr)->last_time = time;
}

int aileron_is_actuator()
{
	return 1;
}

void* aileron_init(int pos)
{
	aileron_t* aileron;

	aileron = malloc(sizeof(aileron_t));

	if (aileron)
	{
		printf("Aileron %s:\tOK\n", pos == 1 ? "right" : "left");

		aileron->current_angle = 0;
		aileron->direction = 0;
		aileron->max_top_angle = 30;
		aileron->max_low_angle = -30;
		aileron->speed = 0.5;
		aileron->pos = pos;
		aileron->last_time = 0;

		aileron->actions.get_module_name = aileron_get_module_name;
		aileron->actions.is_actuator = aileron_is_actuator;
		aileron->actions.sensor.display = aileron_display;
		aileron->actions.sensor.get_float_param = aileron_get_float_param;
		aileron->actions.sensor.set_float_param = aileron_set_float_param;
		aileron->actions.sensor.get_int_param = aileron_get_int_param;
		aileron->actions.sensor.set_int_param = aileron_set_int_param;
		aileron->actions.actuator.activate = aileron_activate;
		aileron->actions.actuator.action = aileron_action;
	}
	else
		printf("Ailerons:\tFAIL\n");

	return aileron;
}

void aileron_release(void* aileron)
{
	unit_t* ptr = (unit_t*)aileron;

	if (!ptr || strcmp(ptr->get_module_name(), aileron_get_module_name()) != 0)
		return;

	free(aileron);
}
