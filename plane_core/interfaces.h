#pragma once

typedef void (*func_get)(void*, const char*, float*);
typedef void (*func_set)(void*, const char*, float);
typedef void (*func_disp)(void*, int update);
typedef const char* (*func_id)();

typedef struct interface
{
	func_get get_param;
	func_set set_param;
	func_disp display;
	func_id get_module_name;
} interface_t;