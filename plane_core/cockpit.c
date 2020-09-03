#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <conio.h>

#include "cockpit.h"

void cockpit_display(void* module, int update);
void cockpit_get_param(void* module, const char* param_name, float* out);
void cockpit_set_param(void* module, const char* param_name, float in);
const char* cockpit_get_module_name();

void* cockpit_init()
{
	cockpit_t* cockpit;

	cockpit = malloc(sizeof(cockpit_t));

	if (cockpit)
	{
		printf("Cockpit:\tOK\n");
		cockpit->tas = 800;
		cockpit->heading = 90;
		cockpit->bank = 0;
		cockpit->aileron_left = 0;
		cockpit->aileron_right = 0;
		cockpit->load = 0;
		cockpit->start_flight = (long)clock();

		cockpit->elements = 0;
		cockpit->elements_count = 0;

		cockpit->functions.display = cockpit_display;
		cockpit->functions.get_param = cockpit_get_param;
		cockpit->functions.set_param = cockpit_set_param;
		cockpit->functions.get_module_name = cockpit_get_module_name;
	}
	else
		printf("Cockpit:\tFAIL\n");

	return cockpit;
}

void cocpit_realese(void* cockpit)
{
	if (cockpit)
	{
		if (((cockpit_t*)cockpit)->elements)
			free(((cockpit_t*)cockpit)->elements);

		free(cockpit);
	}
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
}

void read_sensors(void* cockpit)
{
	cockpit_t* ptr = (cockpit_t*)cockpit;

	for (int i = 0; i < ptr->elements_count; ++i)
	{
		const char* elem_name = ((interface_t*)ptr->elements[i])->get_module_name();

		if (strcmp(elem_name, "aileron") == 0)
		{
			float pos = 0;
			float value = 0;
			((interface_t*)ptr->elements[i])->get_param(ptr->elements[i], "position", &pos);
			((interface_t*)ptr->elements[i])->get_param(ptr->elements[i], "angle", &value);
			if (pos == 1)
				ptr->aileron_right = value;
			else
				ptr->aileron_left = value;
		}
	}
}

void cockpit_display(void* module, int update)
{
	long now;
	long delta;

	now = (long)clock();

	delta = now - ((cockpit_t*)module)->start_flight;

	calc_flight_time((long)delta, &((cockpit_t*)module)->time);

	if (update)
		printf("\033[F\033[F\033[F\033[F\033[F\033[F");

	printf("Flight time:\t%02d:%02d:%02d %04d\n", ((cockpit_t*)module)->time.hour, ((cockpit_t*)module)->time.min, ((cockpit_t*)module)->time.sec, ((cockpit_t*)module)->time.msec);
	printf("TAS:\t\t%03.1f m/s\n", ((cockpit_t*)module)->tas);
	printf("Heading:\t%03.1f\n", ((cockpit_t*)module)->heading);
	printf("Bank:\t\t%02.1f\n", ((cockpit_t*)module)->bank);
	printf("Aileron:\tleft: %02.1f\tright: %02.1f\n", ((cockpit_t*)module)->aileron_left, ((cockpit_t*)module)->aileron_right);
	printf("Load:\t\t%01.1f\n", ((cockpit_t*)module)->load);
}

void cockpit_get_param(void* module, const char* param_name, float* out)
{
	return;
}

void cockpit_set_param(void* module, const char* param_name, float in)
{
	return;
}

void add_dependency(void* cockpit, void* elem)
{
	cockpit_t* ptr = (cockpit_t*)cockpit;

	ptr->elements_count += 1;

	if (ptr->elements)
	{
		ptr->elements = realloc(ptr->elements, sizeof(void*) * ptr->elements_count);
	}
	else
	{
		ptr->elements = malloc(sizeof(void*));
	}
	ptr->elements[ptr->elements_count - 1] = elem;
}

const char* cockpit_get_module_name()
{
	return "cockpit";
}