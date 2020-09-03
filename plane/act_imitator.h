#pragma once

void* actuator_imitator_init();
void actuator_imitator_release(void* act_imitator);

void actuator_imitator_actuator_connect(void* act_imitator, void* actuator);
void actuator_imitator_cockpit_connect(void* act_imitator, void* cockpit);

void actuator_imitator_run_action(void* act_imitator);
