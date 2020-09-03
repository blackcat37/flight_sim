#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "interface.h"
#include "compass.h"

typedef struct compass
{
	unit_t actions;

	float heading;
	float target;
	float turn_speed;

	int last_time;

} compass_t;

const char* compass_get_module_name()
{
	return "compass";
}

void compass_get_float_param(void* compass, const char* param_name, float* out)
{
	unit_t* ptr = (unit_t*)compass;

	if (!ptr || strcmp(ptr->get_module_name(), compass_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "heading") == 0)
	{
		*out = ((compass_t*)ptr)->heading;
	}
	else if (strcmp(param_name, "target") == 0)
	{
		*out = ((compass_t*)ptr)->target;
	}
	else if (strcmp(param_name, "speed") == 0)
	{
		*out = ((compass_t*)ptr)->turn_speed;
	}
}

void compass_get_int_param(void* compass, const char* param_name, int* out)
{
	return;
}

void compass_set_float_param(void* compass, const char* param_name, float in)
{
	unit_t* ptr = (unit_t*)compass;

	if (!ptr || strcmp(ptr->get_module_name(), compass_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "heading") == 0)
	{
		((compass_t*)ptr)->heading = in;
	}
	else if (strcmp(param_name, "target") == 0)
	{
		((compass_t*)ptr)->target = in;
	}
	else if (strcmp(param_name, "speed") == 0)
	{
		((compass_t*)ptr)->turn_speed = in;
	}
}

void compass_set_int_param(void* compass, const char* param_name, int in)
{
	return;
}

void compass_display(void* compass, int update)
{
	unit_t* ptr = (unit_t*)compass;

	if (!ptr || strcmp(ptr->get_module_name(), compass_get_module_name()) != 0)
		return;

	const char* bright = "\x1B[97m";
	const char* normal = "\033[0m";
	const char* highlight;

	if (((compass_t*)compass)->heading != ((compass_t*)compass)->target)
		highlight = bright;
	else
		highlight = normal;

	printf("\033[2KCompass:\theading: %02.2f\ttarget:%s %02.2f%s", ((compass_t*)ptr)->heading, highlight, ((compass_t*)ptr)->target, normal);
}

void compass_activate(void* compass, int plane_speed_int, int bank_angle_int) //base value should be multiplied by 1000 before 
{
	unit_t* ptr = (unit_t*)compass;

	if (!ptr || strcmp(ptr->get_module_name(), compass_get_module_name()) != 0)
		return;

	float plane_speed = plane_speed_int / 1000.0f;
	float bank_angle = bank_angle_int / 1000.0f;

	((compass_t*)compass)->turn_speed = (float)(((9.81 * tan(bank_angle * 0.01745)) / plane_speed) * 57.296);
}

void compass_action(void* compass, int time)
{
	unit_t* ptr = (unit_t*)compass;

	if (!ptr || strcmp(ptr->get_module_name(), compass_get_module_name()) != 0)
		return;

	float delta;
	float shift;
	delta = (time - ((compass_t*)ptr)->last_time) / 1000.0f;
	shift = ((compass_t*)ptr)->turn_speed * delta;

	((compass_t*)ptr)->heading += shift;

	if (((compass_t*)ptr)->heading > 360.0f)
		((compass_t*)ptr)->heading = ((compass_t*)ptr)->heading - 360;
	if (((compass_t*)ptr)->heading < 0.0f)
		((compass_t*)ptr)->heading = 360 - ((compass_t*)ptr)->heading;

	((compass_t*)ptr)->last_time = time;
}

int compass_is_actuator()
{
	return 1;
}

void* compass_init()
{
	compass_t* compass;
	compass = malloc(sizeof(compass_t));
	if (compass)
	{
		printf("Compass:\tOK\n");

		compass->heading = 0;
		compass->last_time = 0;
		compass->target = 0;
		compass->turn_speed = 0;

		compass->actions.get_module_name = compass_get_module_name;
		compass->actions.is_actuator = compass_is_actuator;
		compass->actions.actuator.action = compass_action;
		compass->actions.actuator.activate = compass_activate;
		compass->actions.sensor.display = compass_display;
		compass->actions.sensor.get_float_param = compass_get_float_param;
		compass->actions.sensor.get_int_param = compass_get_int_param;
		compass->actions.sensor.set_float_param = compass_set_float_param;
		compass->actions.sensor.set_int_param = compass_set_int_param;
	}
	else
		printf("Compass:\tFAIL\n\n");

	return compass;
}

void compass_release(void* compass)
{
	unit_t* ptr = (unit_t*)compass;

	if (!ptr || strcmp(ptr->get_module_name(), compass_get_module_name()) != 0)
		return;

	free(compass);
}