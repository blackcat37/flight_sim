#include <stdlib.h>
#include <stdio.h>

#include "interface.h"
#include "act_imitator.h"

typedef struct actuators
{
	void** actuators;
	int actuators_cnt;
	void* cockpit;
} actuators_t;

void* actuator_imitator_init()
{
	actuators_t* act = malloc(sizeof(actuators_t));
	if (act)
	{
		act->actuators_cnt = 0;
		act->actuators = 0;
		act->cockpit = 0;
	}

	return act;
}

void actuator_imitator_release(void* act_imitator)
{
	actuators_t* ptr = (actuators_t*)act_imitator;

	if (!ptr)
		return;

	if (ptr->actuators)
			free(ptr->actuators);
	
	free(ptr);
}

void actuator_imitator_actuator_connect(void* act_imitator, void* actuator)
{
	actuators_t* ptr = (actuators_t*)act_imitator;

	if (!ptr)
		return;

	int elems = ptr->actuators_cnt;
	ptr->actuators_cnt += 1;

	if (elems)
	{
		ptr->actuators = realloc(ptr->actuators, sizeof(void*) * ptr->actuators_cnt);
	}
	else
	{
		ptr->actuators = malloc(sizeof(void*));
	}
	ptr->actuators[ptr->actuators_cnt - 1] = actuator;

	printf("Simulator:\tunit \"%s\" connected!\n", ((unit_t*)actuator)->get_module_name());
}

void actuator_imitator_cockpit_connect(void* act_imitator, void* cockpit)
{
	actuators_t* ptr = act_imitator;
	if (!ptr || !cockpit)
		return;

	ptr->cockpit = cockpit;
}

void actuator_imitator_run_action(void* act_imitator)
{
	actuators_t* ptr = act_imitator;
	if (!ptr)
		return;

	int time;
	if (ptr->cockpit)
		((unit_t*)ptr->cockpit)->sensor.get_int_param(ptr->cockpit, "time", &time);
	else
		return;

	for (int i = 0; i < ptr->actuators_cnt; ++i)
	{
		unit_t* act = (unit_t*)ptr->actuators[i];
		if (act->is_actuator())
			act->actuator.action(act, time);
	}
}