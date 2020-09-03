#include <stdio.h>
#include <conio.h>

#include "cockpit.h"
#include "aileron.h"

void* cockpit;
void* aileron_left;
void* aileron_right;

int main_loop()
{
	while (1 && !_kbhit())
	{
		//read states

		//show states
		((interface_t*)cockpit)->display(cockpit, 1);
		//cocpit->display(cockpit, 1);
	}

	return 0;
}

int main(void)
{
	printf("\n");
	printf("\x1B[31mTexting\033[0m\t\t");
	printf("\x1B[32mTexting\033[0m\t\t");
	printf("\x1B[33mTexting\033[0m\t\t");
	printf("\x1B[34mTexting\033[0m\t\t");
	printf("\x1B[35mTexting\033[0m\n");

	printf("\x1B[36mTexting\033[0m\t\t");
	printf("\x1B[36mTexting\033[0m\t\t");
	printf("\x1B[36mTexting\033[0m\t\t");
	printf("\x1B[37mTexting\033[0m\t\t");
	printf("\x1B[93mTexting\033[0m\n");

	printf("\033[3;42;30mTexting\033[0m\t\t");
	printf("\033[3;43;30mTexting\033[0m\t\t");
	printf("\033[3;44;30mTexting\033[0m\t\t");
	printf("\033[3;104;30mTexting\033[0m\t\t");
	printf("\033[3;100;30mTexting\033[0m\n");

	printf("\033[3;47;35mTexting\033[0m\t\t");
	printf("\033[2;47;35mTexting\033[0m\t\t");
	printf("\033[1;47;35mTexting\033[0m\t\t");
	printf("\t\t");
	printf("\n");

	//printf("Plane init:\n");
	//aileron_left = aileron_init(-1);
	//aileron_right = aileron_init(1);
	//printf("Rudder:\t\tOK\n");
	//cockpit = cockpit_init();
	//add_dependency(cockpit, aileron_left);
	//add_dependency(cockpit, aileron_right);
	//printf("Helm:\t\tOK\n");
	//printf("\n\nTaking off....\n");

	//printf("\033[?25l"); //printf("\033[?25h");
	////print_state(0, 0);

	//printf("\x1B[33m\n\n\n\n\n\n\n");

	//main_loop();

	//cocpit_realese(cockpit);
	//aileron_release(aileron_left);
	//aileron_release(aileron_right);

	return 0;
}