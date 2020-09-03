#include <conio.h>
#include <stdio.h>

#include "interface.h"
#include "cockpit.h"
#include "aileron.h"
#include "gyro.h"
#include "compass.h"
#include "pito.h"

#include "flight_comp.h"
#include "act_imitator.h"

int main_loop(void* flight_computer, void* cockpit, void* actuator_imitator)
{
	int cont = 1;

	while (cont)
	{
		((unit_t*)cockpit)->sensor.display(cockpit, 1);

		//flight task
		flight_computer_display(flight_computer);

		flight_computer_run_task(flight_computer);

		if (_kbhit())
		{
			int c = _getch();
			if (c == 27)
				cont = 0;
			else
				flight_computer_input_char(flight_computer, c);
		}

		actuator_imitator_run_action(actuator_imitator); // imitation of the actuators work
	}

	return 0;
}

int main(void)
{
	void* flight_computer;
	void* actuator_imitator;

	void* cockpit;
	void* l_aileron;
	void* r_aileron;
	void* gyro;
	void* compass;
	void* pito;

	pito = pito_init();
	gyro = gyro_init();
	compass = compass_init();
	l_aileron = aileron_init(-1);
	r_aileron = aileron_init(1);

	cockpit = cockpit_init();

	flight_computer = flight_computer_init();

	cockpit_unit_connect(cockpit, pito);
	cockpit_unit_connect(cockpit, compass);
	cockpit_unit_connect(cockpit, gyro);
	cockpit_unit_connect(cockpit, l_aileron);
	cockpit_unit_connect(cockpit, r_aileron);

	flight_computer_unit_connect(flight_computer, l_aileron);
	flight_computer_unit_connect(flight_computer, r_aileron);
	flight_computer_unit_connect(flight_computer, cockpit);
	flight_computer_unit_connect(flight_computer, compass);
	flight_computer_unit_connect(flight_computer, gyro);
	flight_computer_unit_connect(flight_computer, pito);

	actuator_imitator = actuator_imitator_init();
	actuator_imitator_actuator_connect(actuator_imitator, cockpit);
	actuator_imitator_actuator_connect(actuator_imitator, l_aileron);
	actuator_imitator_actuator_connect(actuator_imitator, r_aileron);
	actuator_imitator_actuator_connect(actuator_imitator, gyro);
	actuator_imitator_actuator_connect(actuator_imitator, pito);
	actuator_imitator_actuator_connect(actuator_imitator, compass);
	actuator_imitator_cockpit_connect(actuator_imitator, cockpit);

	printf("\n\n\n\n\n\n");
	printf("\033[?25l"); //printf("\033[?25h");

	//printf("%d\n\n", _getch());

	main_loop(flight_computer, cockpit, actuator_imitator);

	cocpit_realese(cockpit);
	aileron_release(l_aileron);
	aileron_release(r_aileron);
	gyro_release(gyro);
	compass_release(compass);
	pito_release(pito);
	actuator_imitator_release(actuator_imitator);
	flight_computer_release(flight_computer);

	return 0;
}