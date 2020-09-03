#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "interface.h"
#include "gyro.h"

typedef struct gyro
{
	unit_t actions;

	float current_bank_angle;
	float bank_speed;

	float load;

	int last_time;
} gyro_t;

const char* gyro_get_module_name()
{
	return "gyro";
}

void gyro_get_float_param(void* gyro, const char* param_name, float* out)
{
	unit_t* ptr = (unit_t*)gyro;

	if (!ptr || strcmp(ptr->get_module_name(), gyro_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "angle") == 0)
	{
		*out = ((gyro_t*)ptr)->current_bank_angle;
	}
	else if (strcmp(param_name, "speed") == 0)
	{
		*out = ((gyro_t*)ptr)->bank_speed;
	}
	else if (strcmp(param_name, "load") == 0)
	{
		*out = ((gyro_t*)ptr)->load;
	}
}

void gyro_get_int_param(void* gyro, const char* param_name, int* out)
{
	return;
}

void gyro_set_float_param(void* gyro, const char* param_name, float in)
{
	return;
}

void gyro_set_int_param(void* gyro, const char* param_name, int in)
{
	return;
}

void gyro_display(void* gyro, int update)
{
	unit_t* ptr = (unit_t*)gyro;

	if (!ptr || strcmp(ptr->get_module_name(), gyro_get_module_name()) != 0)
		return;

	const char* red = "\x1B[91m";
	const char* yellow = "\x1B[93m";
	const char* normal = "\033[0m";

	const char* highlight;

	if (((gyro_t*)gyro)->load <= 1.3)
		highlight = normal;
	else if (((gyro_t*)gyro)->load > 1.3 && ((gyro_t*)gyro)->load <= 1.5)
		highlight = yellow;
	else
		highlight = red;

	printf("\033[2KGyro:\t\tbank: %02.2f\tpith: 00.00\tload:%s %2.2f%s", ((gyro_t*)gyro)->current_bank_angle, highlight, ((gyro_t*)gyro)->load, normal);
}

void gyro_activate(void* gyro, int param_1, int aileron_angle_int) //base value should be multiplied by 1000 before 
{
	unit_t* ptr = (unit_t*)gyro;

	if (!ptr || strcmp(ptr->get_module_name(), gyro_get_module_name()) != 0)
		return;

	float aileron_angle = aileron_angle_int / 1000.0f;

	float aileron_efficiency = 0.5f; // aileron efficiency
	float damping_factor = 2.0f; // damping factor

	((gyro_t*)gyro)->bank_speed = (aileron_efficiency / damping_factor) * aileron_angle;
}

void gyro_action(void* gyro, int time)
{
	unit_t* ptr = (unit_t*)gyro;

	if (!ptr || strcmp(ptr->get_module_name(), gyro_get_module_name()) != 0)
		return;

	float delta;
	float shift;
	delta = (time - ((gyro_t*)ptr)->last_time) / 1000.0f;
	shift = ((gyro_t*)ptr)->bank_speed * delta;

	((gyro_t*)ptr)->current_bank_angle += shift;
	((gyro_t*)ptr)->load = (float)(1 / cos(((gyro_t*)ptr)->current_bank_angle * 0.01745));
	((gyro_t*)ptr)->last_time = time;
}

int gyro_is_actuator()
{
	return 1;
}

void* gyro_init()
{
	gyro_t* gyro = malloc(sizeof(gyro_t));

	if (gyro)
	{
		printf("Gyro:\t\tOK\n");

		gyro->bank_speed = 0;
		gyro->current_bank_angle = 0;
		gyro->last_time = 0;
		gyro->load = 1;

		gyro->actions.get_module_name = gyro_get_module_name;
		gyro->actions.is_actuator = gyro_is_actuator;
		gyro->actions.actuator.action = gyro_action;
		gyro->actions.actuator.activate = gyro_activate;
		gyro->actions.sensor.display = gyro_display;
		gyro->actions.sensor.get_float_param = gyro_get_float_param;
		gyro->actions.sensor.get_int_param = gyro_get_int_param;
		gyro->actions.sensor.set_float_param = gyro_set_float_param;
		gyro->actions.sensor.set_int_param = gyro_set_int_param;
	}
	else
		printf("Gyro:\t\tFAIL\n\n");

	return gyro;

}

void gyro_release(void* gyro)
{
	unit_t* ptr = (unit_t*)gyro;

	if (!ptr || strcmp(ptr->get_module_name(), gyro_get_module_name()) != 0)
		return;

	free(gyro);
}