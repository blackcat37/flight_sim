#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#include "aileron.h"

void aileron_display(void* module, int update);
void aileron_get_param(void* module, const char* param_name, float* out);
void aileron_set_param(void* module, const char* param_name, float in);
const char* aileron_get_module_name();

void* aileron_init(int pos)
{
	aileron_t* aileron;

	aileron = malloc(sizeof(aileron_t));

	if (aileron)
	{
		printf("Aileron %s:\tOK\n", pos == 1 ? "right" : "left");

		aileron->current_angle = 0;
		aileron->dest_angle = 0;
		aileron->state = 0;
		aileron->max_top_angle = 30;
		aileron->max_low_angle = -30;
		aileron->speed = 0.5;
		aileron->pos = pos;

		aileron->functions.display = aileron_display;
		aileron->functions.get_param = aileron_get_param;
		aileron->functions.set_param = aileron_set_param;
		aileron->functions.get_module_name = aileron_get_module_name;
	}
	else
		printf("Ailerons:\tFAIL\n");

	return aileron;
}

void aileron_release(void* aileron)
{
	if (aileron)
		free(aileron);
}

void aileron_display(void* module, int update)
{
	return;
}

const char* aileron_get_module_name()
{
	return "aileron";
}

void aileron_get_param(void* module, const char* param_name, float* out)
{
	aileron_t* ptr = (aileron_t*)module;

	if (strcmp(param_name, "angle") == 0)
	{
		if (ptr->pos==-1)
			*out = ptr->current_angle + 100;
		else
			*out = ptr->current_angle - 100;
	}
	else if (strcmp(param_name, "position") == 0)
	{
		*out = (float)ptr->pos;
	}

	return;
}

void aileron_set_param(void* module, const char* param_name, float in)
{
	return;
}