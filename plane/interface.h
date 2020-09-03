#pragma once

typedef void (*func_get_float)(void*, const char*, float*);
typedef void (*func_get_int)(void*, const char*, int*);
typedef void (*func_set_float)(void*, const char*, float);
typedef void (*func_set_int)(void*, const char*, int);
typedef void (*func_display)(void*, int);

typedef void (*func_activate)(void*, int, int);
typedef void (*func_action)(void*, int);

typedef int (*func_is_actuator)();
typedef const char* (*func_get_name)();

typedef struct sensor
{
	func_get_float get_float_param;
	func_set_float set_float_param;
	func_get_int get_int_param;
	func_set_int set_int_param;
	func_display display;
} sensor_t;

typedef struct actuator
{
	func_activate activate;
	func_action action;
} actuator_t;

typedef struct unit
{
	sensor_t sensor;
	actuator_t actuator;

	func_is_actuator is_actuator;
	func_get_name get_module_name;
} unit_t;

