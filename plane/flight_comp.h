#pragma once

void* flight_computer_init();
void flight_computer_release(void* flight_computer);

void flight_computer_unit_connect(void* flight_computer, void* unit);

void flight_computer_input_char(void* flight_computer, int c);

void flight_computer_display(void* flight_computer);

void flight_computer_run_task(void* flight_computer);