#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "interface.h"
#include "cockpit.h"

typedef struct daytime
{
	int hour;
	int min;
	int sec;
	int msec;
	int in_msec;
} daytime_t;

typedef struct cockpit
{
	unit_t actions;

	daytime_t time;
	long start_flight;

	void** elements;
	int elements_count;
} cockpit_t;

const char* cockpit_get_module_name()
{
	return "cockpit";
}

void cockpit_get_float_param(void* cockpit, const char* param_name, float* out)
{
	return;
}

void cockpit_get_int_param(void* cockpit, const char* param_name, int* out)
{
	unit_t* ptr = (unit_t*)cockpit;

	if (ptr && strcmp(ptr->get_module_name(), cockpit_get_module_name()) == 0)
	{
		if (strcmp(param_name, "time") == 0)
			*out = ((cockpit_t*)ptr)->time.in_msec;
	}
}

void cockpit_set_float_param(void* cockpit, const char* param_name, float in)
{
	return;
}

void cockpit_set_int_param(void* cockpit, const char* param_name, int in)
{
	return;
}

void calc_flight_time(long tics, daytime_t* out)
{
	double d_ms = (double)tics / CLOCKS_PER_SEC;
	long ms = (long)(d_ms * 1000);

	int full_secs = ms / 1000;
	int msec = ms - full_secs * 1000;
	int hour = full_secs / 3600;
	full_secs = full_secs - hour * 3600;
	int min = full_secs / 60;
	full_secs = full_secs - min * 60;

	out->hour = hour;
	out->min = min;
	out->sec = full_secs;
	out->msec = msec;
	out->in_msec = ms;
}

void cockpit_display(void* cockpit, int update)
{
	unit_t* ptr = (unit_t*)cockpit;

	if (!ptr || strcmp(ptr->get_module_name(), cockpit_get_module_name()) != 0)
		return;

	if (update)
	{
		for (int i = 0; i < ((cockpit_t*)ptr)->elements_count; ++i)
			printf("\033[F");
	}

	printf("Flight time:\t%02d:%02d:%02d %04d\n", ((cockpit_t*)ptr)->time.hour, ((cockpit_t*)ptr)->time.min, ((cockpit_t*)ptr)->time.sec, ((cockpit_t*)ptr)->time.msec);

	for (int i = 0; i < ((cockpit_t*)ptr)->elements_count; ++i)
	{
		if (strcmp(((unit_t*)((cockpit_t*)ptr)->elements[i])->get_module_name(), "aileron") == 0)
		{
			int pos = 0;
			((unit_t*)((cockpit_t*)ptr)->elements[i])->sensor.get_int_param(((cockpit_t*)ptr)->elements[i], "position", &pos);
			((unit_t*)((cockpit_t*)ptr)->elements[i])->sensor.display(((cockpit_t*)ptr)->elements[i], 1);
			if (pos == 1)
				printf("\n");
		}
		else
		{
			((unit_t*)((cockpit_t*)ptr)->elements[i])->sensor.display(((cockpit_t*)ptr)->elements[i], 1);
			printf("\n");
		}
	}
}

int cockpit_is_actuator()
{
	return 1;
}

void cockpit_activate(void* cockpit, int time, int param)
{
	return;
}

void cockpit_action(void* cockpit, int time)
{
	long now;
	long delta;

	unit_t* ptr = (unit_t*)cockpit;

	if (!ptr || strcmp(ptr->get_module_name(), cockpit_get_module_name()) != 0)
		return;

	now = (long)clock();

	delta = now - ((cockpit_t*)ptr)->start_flight;

	calc_flight_time((long)delta, &((cockpit_t*)ptr)->time);
}

void* cockpit_init()
{
	cockpit_t* cockpit;

	cockpit = malloc(sizeof(cockpit_t));

	if (cockpit)
	{
		printf("Cockpit:\tOK\n\n");
		cockpit->start_flight = (long)clock();

		cockpit->elements = 0;
		cockpit->elements_count = 0;

		cockpit->time.hour = 0;
		cockpit->time.in_msec = 0;
		cockpit->time.min = 0;
		cockpit->time.msec = 0;
		cockpit->time.sec = 0;

		cockpit->actions.is_actuator = cockpit_is_actuator;

		cockpit->actions.get_module_name = cockpit_get_module_name;
		cockpit->actions.sensor.display = cockpit_display;
		cockpit->actions.sensor.get_float_param = cockpit_get_float_param;
		cockpit->actions.sensor.set_float_param = cockpit_set_float_param;
		cockpit->actions.sensor.get_int_param = cockpit_get_int_param;
		cockpit->actions.sensor.set_int_param = cockpit_set_int_param;
		cockpit->actions.actuator.action = cockpit_action;
		cockpit->actions.actuator.activate = cockpit_activate;
	}
	else
		printf("Cockpit:\tFAIL\n");

	return cockpit;
}

void cocpit_realese(void* cockpit)
{
	unit_t* ptr = (unit_t*)cockpit;

	if (!ptr || strcmp(ptr->get_module_name(), cockpit_get_module_name()) != 0)
		return;

	if (((cockpit_t*)cockpit)->elements)
		free(((cockpit_t*)cockpit)->elements);
	
	free(cockpit);
}

void cockpit_unit_connect(void* cockpit, void* elem)
{
	cockpit_t* ptr = (cockpit_t*)cockpit;

	if (!ptr || strcmp(ptr->actions.get_module_name(), cockpit_get_module_name()) != 0)
		return;

	int elems = ptr->elements_count;
	ptr->elements_count += 1;

	if (elems)
	{
		ptr->elements = realloc(ptr->elements, sizeof(void*) * ptr->elements_count);
	}
	else
	{
		ptr->elements = malloc(sizeof(void*));
	}
	ptr->elements[ptr->elements_count - 1] = elem;

	printf("Cockpit:\tunit \"%s\" connected!\n", ((unit_t*)elem)->get_module_name());
}