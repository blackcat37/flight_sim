#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "flight_comp.h"
#include "interface.h"

typedef float (*func_pid_update)(void*, float, int);
typedef int (*func_task)(void*);

typedef struct pid_regulator
{
	float P_factor;
	float I_factor;
	float D_factor;

	float P;
	float I;
	float D;

	float last_error;
	int last_time;

	func_pid_update update;
} pid_regulator_t;

typedef struct filght_computer
{
	unit_t* l_aileron;
	unit_t* r_aileron;
	unit_t* cockpit;
	unit_t* compass;
	unit_t* gyro;
	unit_t* pito;

	char* user_input;
	int user_input_size;
	int user_input_cnt;

	func_task* task_queue;
	int task_queue_size;
	int task_queue_cnt;

	pid_regulator_t* pid_bank;
	pid_regulator_t* pid_heading;

	int last_task_result;
	int exe_count;

	float pid_heading_result;
} flight_computer_t;

void pid_init(pid_regulator_t* pid, float P_factor, float I_factor, float D_factor)
{
	pid->P_factor = P_factor;
	pid->I_factor = I_factor;
	pid->D_factor = D_factor;

	pid->P = 0;
	pid->I = 0;
	pid->D = 0;
	pid->last_error = 0;
	pid->last_time = 0;
}

float pid_update(pid_regulator_t* pid, float error, int time)
{
	float delta_time;
	delta_time = (time - pid->last_time) / 1000.0f;

	pid->P = error;
	pid->I += error * delta_time;
	pid->D = (error - pid->last_error) / delta_time;

	float res;
	res = pid->P * pid->P_factor + pid->I * pid->I_factor + pid->D * pid->D_factor;

	pid->last_error = error;
	pid->last_time = time;

	return res;
}

void flight_computer_read_sensors(void* flight_computer, int* time, int* aileron_position, float* aileron_angle, float* bank_angle, float* plane_speed, float* heading)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	if (!ptr)
		return;

	ptr->cockpit->sensor.get_int_param(ptr->cockpit, "time", time);
	ptr->l_aileron->sensor.get_float_param(ptr->l_aileron, "angle", aileron_angle);
	ptr->l_aileron->sensor.get_int_param(ptr->l_aileron, "position", aileron_position);
	ptr->gyro->sensor.get_float_param(ptr->gyro, "angle", bank_angle);
	ptr->compass->sensor.get_float_param(ptr->compass, "heading", heading);
	ptr->pito->sensor.get_float_param(ptr->pito, "speed", plane_speed);
}

void flight_computer_activate_actuator(void* flight_computer, int aileron_move_direction, float aileron_angle, float bank_angle, float plane_speed)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	if (!ptr)
		return;

	ptr->l_aileron->actuator.activate(ptr->l_aileron, 0, aileron_move_direction);
	ptr->r_aileron->actuator.activate(ptr->r_aileron, 0, aileron_move_direction * -1);
	ptr->gyro->actuator.activate(ptr->gyro, 0, (int)(aileron_angle * 1000));
	ptr->compass->actuator.activate(ptr->compass, (int)(plane_speed * 1000), (int)(bank_angle * 1000));
}

int bank(void* flight_computer, int time, int aileron_position, float aileron_angle, float bank_angle, float target);
float calc_angle_error(float heading, float target);
float adjust_target_bank_angle(float target_bank_angle, float plane_speed);

int task_adjust_ailerons_to_zero_angle(void* flight_computer)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	if (!ptr)
		return 0;

	float aileron_angle;
	int time;
	int aileron_pos;
	float bank_angle;
	float plane_speed;
	float heading;

	int aileron_move_direction = 0;

	flight_computer_read_sensors(flight_computer, &time, &aileron_pos, &aileron_angle, &bank_angle, &plane_speed, &heading);

	if (((int)(aileron_angle * 10)) != 0)
	{
		if (aileron_angle < 0)
			aileron_move_direction = -1 * aileron_pos;
	}
	else
		aileron_move_direction = 0;

	flight_computer_activate_actuator(flight_computer, aileron_move_direction, aileron_angle, bank_angle, plane_speed);

	return aileron_move_direction;
}

int task_turn_new_heading(void* flight_computer)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	if (!ptr)
		return 0;

	float aileron_angle;
	int time;
	int aileron_pos;
	float bank_angle;
	float plane_speed;
	float heading;
	float target;

	flight_computer_read_sensors(flight_computer, &time, &aileron_pos, &aileron_angle, &bank_angle, &plane_speed, &heading);
	ptr->compass->sensor.get_float_param(ptr->compass, "target", &target);

	if (((int)(target * 10)) == ((int)(heading * 10)) && ((int)(aileron_angle * 10)) < 2 && ((int)(bank_angle * 10)) == 0 )
		return 0;

	float angle_error = calc_angle_error(heading, target);
	float target_bank_angle = adjust_target_bank_angle(angle_error, plane_speed);

	//float imp = ptr->pid_heading->update(ptr->pid_heading, angle_error, time);
	//target_bank_angle = imp;

	//if (imp > target_bank_angle)
	//	target_bank_angle = angle_error;
	//if (imp < (target_bank_angle * -1.0f))
	//	target_bank_angle = angle_error * -1.0f;

	int aileron_move_direction = bank(flight_computer, time, aileron_pos, aileron_angle, bank_angle, target_bank_angle);

	flight_computer_activate_actuator(flight_computer, aileron_move_direction, aileron_angle, bank_angle, plane_speed);

	return 1;
}

int bank(void* flight_computer, int time, int aileron_position, float aileron_angle, float bank_angle, float target)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	if (!ptr)
		return 0;

	if (((int)(bank_angle * 10)) == ((int)(target * 10)) && ((int)(aileron_angle * 10)) <= 3)
		return 0;

	float angle_error;
	int aileron_move_direction = aileron_position;

	angle_error = bank_angle - target;

	float imp = pid_update(ptr->pid_bank, angle_error, time);

	if (imp != 0)
	{
		if (imp < 0)
			aileron_move_direction *= -1;
	}
	else
		aileron_move_direction = 0;

	return aileron_move_direction;
}

float calc_angle_error(float heading, float target)
{
	float angle_error = 0;
	float m = -1.0f;

	angle_error = (heading - target) * m;

	return angle_error;
}

float adjust_target_bank_angle(float target_bank_angle, float plane_speed)
{
	float result = target_bank_angle;
	if (result == 0)
		return result;

	float turn_speed = (float)(((9.81 * tan(target_bank_angle * 0.01745)) / plane_speed) * 57.296);
	float load = (float)(1 / cos(target_bank_angle * 0.01745));

	if (fabs(turn_speed) > 3.0f || fabs(load) > 1.5f)
	{
		if (result < 0)
			result = -25.0f;
		else
			result = 25.0f;
	}

	return result;
}

void* flight_computer_init()
{
	flight_computer_t* flc = malloc(sizeof(flight_computer_t));

	if (flc)
	{
		printf("Flight computer:\tOK\n");
		flc->l_aileron = 0;
		flc->r_aileron = 0;
		flc->cockpit = 0;
		flc->compass = 0;
		flc->gyro = 0;
		flc->pito = 0;

		flc->user_input = malloc(sizeof(char) * 100);
		if (flc->user_input)
		{
			memset(flc->user_input, 0, sizeof(char) * 100);
			flc->user_input_cnt = 0;
			flc->user_input_size = 100;
		}

		flc->task_queue = malloc(sizeof(func_task) * 3);
		if (flc->task_queue)
		{
			memset(flc->task_queue, 0, sizeof(void*) * 3);
			flc->task_queue[0] = task_adjust_ailerons_to_zero_angle;
			flc->task_queue_cnt = 1;
			flc->task_queue_size = 3;
		}

		flc->pid_bank = malloc(sizeof(pid_regulator_t));
		flc->pid_heading = malloc(sizeof(pid_regulator_t));
		flc->pid_bank->update = pid_update;
		flc->pid_heading->update = pid_update;

		pid_init(flc->pid_bank, 0.25f, 0.0f, 2.5f);
		pid_init(flc->pid_heading, 0.1f, 0.0f, 2.5f);

		flc->last_task_result = 0;
		flc->exe_count = 0;
		flc->pid_heading_result = 0;
	}
	else
		printf("Flight computer:\tFAIL\n");

	return flc;
}

void flight_computer_release(void* flight_computer)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	if (!ptr)
		return;

	if (ptr->user_input)
		free(ptr->user_input);

	if (ptr->task_queue)
		free(ptr->task_queue);

	if (ptr->pid_bank)
		free(ptr->pid_bank);

	if (ptr->pid_heading)
		free(ptr->pid_heading);

	free(flight_computer);
}

void flight_computer_unit_connect(void* flight_computer, void* unit)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;
	unit_t* elem = (unit_t*)unit;

	if (!ptr || !elem)
		return;

	if (strcmp(elem->get_module_name(), "aileron") == 0)
	{
		int pos = 0;
		elem->sensor.get_int_param(unit, "position", &pos);
		if (pos == -1)
			ptr->l_aileron = elem;
		else if (pos == 1)
			ptr->r_aileron = elem;
	}
	else if (strcmp(elem->get_module_name(), "cockpit") == 0)
	{
		ptr->cockpit = elem;
	}
	else if (strcmp(elem->get_module_name(), "compass") == 0)
	{
		ptr->compass = elem;
	}
	else if (strcmp(elem->get_module_name(), "gyro") == 0)
	{
		ptr->gyro = elem;
	}
	else if (strcmp(elem->get_module_name(), "pito") == 0)
	{
		ptr->pito = elem;
	}
}

void flight_computer_input_char(void* flight_computer, int c)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;

	if (!ptr)
		return;

	if (c == 13)
	{
		if (ptr->user_input_cnt != 0)
		{
			float target = (float)atof(ptr->user_input);
			ptr->compass->sensor.set_float_param(ptr->compass, "target", target);
			memset(ptr->user_input, 0, ptr->user_input_size);
			ptr->user_input_cnt = 0;

			ptr->task_queue[0] = task_turn_new_heading;
			ptr->exe_count += 1;
		}
	}
	else if (c == 8)
	{
		if (ptr->user_input_cnt > 0)
		{
			ptr->user_input[ptr->user_input_cnt - 1] = 0;
			ptr->user_input_cnt -= 1;
		}
	}
	else
	{
		int r = ptr->user_input_size - ptr->user_input_cnt;
		if (r == 1)
		{
			ptr->user_input_size *= 2;
			ptr->user_input = realloc(ptr->user_input, sizeof(char) * ptr->user_input_size);
		}

		ptr->user_input[ptr->user_input_cnt] = c;
		ptr->user_input_cnt += 1;
	}
}

void flight_computer_display(void* flight_computer)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;

	if (!ptr)
		return;

	func_task task1 = task_adjust_ailerons_to_zero_angle;
	func_task task2 = task_turn_new_heading;

	char text[64] = { '\0', };

	if (ptr->task_queue_cnt > 0)
	{
		if (ptr->task_queue[0] == task1)
			strcpy_s(text, 64, "task_adjust_ailerons_to_zero_angle");
		else if (ptr->task_queue[0] == task2)
			strcpy_s(text, 64, "task_turn_new_heading");
		else
			strcpy_s(text, 64, "no task");
	}

	printf("\nFlight computer:\n");
	printf("\033[2K\tCurrent task: %s\tCompleted tasks: %d\tPID: %3.3f\n", text, ptr->exe_count, ptr->pid_heading_result);
	printf("\033[2K> %s", ptr->user_input);

	printf("\033[F\033[F\033[F");
}

void flight_computer_run_task(void* flight_computer)
{
	flight_computer_t* ptr = (flight_computer_t*)flight_computer;

	if (!ptr)
		return;

	if (ptr->task_queue_cnt > 0)
	{
		if (ptr->task_queue[0](flight_computer) == 0)
		{
			func_task task = task_turn_new_heading;

			if (ptr->task_queue[0] == task)
			{
				ptr->task_queue[0] = task_adjust_ailerons_to_zero_angle;
				ptr->exe_count += 1;
			}
		}
	}
}
