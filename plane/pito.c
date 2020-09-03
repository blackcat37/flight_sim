#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "interface.h"
#include "pito.h"

typedef struct pito
{
	unit_t actions;

	float speed;

	int last_time;
} pito_t;

const char* pito_get_module_name()
{
	return "pito";
}

void pito_get_float_param(void* pito, const char* param_name, float* out)
{
	unit_t* ptr = (unit_t*)pito;

	if (!ptr || strcmp(ptr->get_module_name(), pito_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "speed") == 0)
	{
		*out = ((pito_t*)ptr)->speed;
	}
}

void pito_get_int_param(void* pito, const char* param_name, int* out)
{
	return;
}

void pito_set_float_param(void* pito, const char* param_name, float in)
{
	unit_t* ptr = (unit_t*)pito;

	if (!ptr || strcmp(ptr->get_module_name(), pito_get_module_name()) != 0)
		return;

	if (strcmp(param_name, "speed") == 0)
	{
		((pito_t*)ptr)->speed = in;
	}
}

void pito_set_int_param(void* pito, const char* param_name, int in)
{
	return;
}

void pito_display(void* pito, int update)
{
	unit_t* ptr = (unit_t*)pito;

	if (!ptr || strcmp(ptr->get_module_name(), pito_get_module_name()) != 0)
		return;

	printf("Plane speed:\t%000.3f m/s", ((pito_t*)ptr)->speed);
}

void pito_activate(void* pito, int param_1, int param_2)
{
	unit_t* ptr = (unit_t*)pito;

	if (!ptr || strcmp(ptr->get_module_name(), pito_get_module_name()) != 0)
		return;
}

void pito_action(void* pito, int time)
{
	unit_t* ptr = (unit_t*)pito;

	if (!ptr || strcmp(ptr->get_module_name(), pito_get_module_name()) != 0)
		return;

	((pito_t*)ptr)->last_time = time;
}

int pito_is_actuator()
{
	return 1;
}

void* pito_init()
{
	pito_t* pito;
	pito = malloc(sizeof(pito_t));

	if (pito)
	{
		printf("Pito tube:\tOK\n");

		pito->last_time = 0;
		pito->speed = 222.0f;

		pito->actions.get_module_name = pito_get_module_name;
		pito->actions.is_actuator = pito_is_actuator;
		pito->actions.actuator.action = pito_action;
		pito->actions.actuator.activate = pito_activate;
		pito->actions.sensor.display = pito_display;
		pito->actions.sensor.get_float_param = pito_get_float_param;
		pito->actions.sensor.get_int_param = pito_get_int_param;
		pito->actions.sensor.set_float_param = pito_set_float_param;
		pito->actions.sensor.set_int_param = pito_set_int_param;
	}
	else
		printf("Pito tube:\tFAIL\n");

	return pito;
}

void pito_release(void* pito)
{
	unit_t* ptr = (unit_t*)pito;

	if (!ptr || strcmp(ptr->get_module_name(), pito_get_module_name()) != 0)
		return;

	free(pito);
}
