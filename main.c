#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libenjoy/src/libenjoy.h"
#include "libtrik.h"

#define SERVO_S1_MIN_DUTY 500000
#define SERVO_S1_MAX_DUTY 2300000

#define SERVO_S2_MIN_DUTY 700000
#define SERVO_S2_MAX_DUTY 2158665

#define SERVO_S3_MIN_DUTY 700000
#define SERVO_S3_MAX_DUTY 1795460

#define STICK_DEADZONE 200

#define JOYSTICK_NAME "PLAYSTATION(R)3 Controller"

enum joy_keys {
	JOY_SELECT,
	JOY_L3,
	JOY_R3,
	JOY_START,
	JOY_UP,
	JOY_RIGHT,
	JOY_DOWN,
	JOY_LEFT,
	JOY_L2,
	JOY_R2,
	JOY_L1,
	JOY_R1,
	JOY_TRIANGLE,
	JOY_CIRCLE,
	JOY_CROSS,
	JOY_SQUARE,
	JOY_PS
};

int axis_state[] = {
	0,
	0,
	0
};

int servo_state[] = {
	500000,
	1400000,
	1400000
};

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
	servo_enable(S1);
	servo_enable(S2);
	servo_enable(S3);

	servo_set(S1, 500000);
	servo_set(S2, 1400000);
	servo_set(S3, 1400000);
}

void destruct() {
	servo_disable(S1);
	servo_disable(S2);
	servo_disable(S3);
}

int main(int argc, char const *argv[]) {
	init_devices();

	libenjoy_context *ctx = libenjoy_init();
	libenjoy_joy_info_list *info;

	libenjoy_enumerate(ctx);

	info = libenjoy_get_info_list(ctx);

	signed char found = -128;
	if (info->count > 0)
	{
		char i;

		for (i = 0; i < info->count; i++) {
			if (strstr(info->list[i]->name, JOYSTICK_NAME) != NULL) {
				found = i;
				printf("Joystick found: %s...\n", info->list[i]->name);
			}
		}

		if (found < 0) {
			fprintf(stderr, "Specified input device not found.\n");
			return -1;
		}
	} else {
		fprintf(stderr, "No input devices found.\n");
		return -1;
	}

	libenjoy_joystick *joy;
	joy = libenjoy_open_joystick(ctx, info->list[found]->id);

	if (joy)
	{
		int counter = 0;
		libenjoy_event ev;

		printf("Connected to %d\n", info->list[found]->id);

		setup();

		while(1)
		{
			while(libenjoy_poll(ctx, &ev))
			{
				switch(ev.type)
				{
					case LIBENJOY_EV_AXIS:
						if (ev.part_id == 1) {
							if (ev.data > STICK_DEADZONE) {
								if (ev.data - axis_state[2] > 0) {
									servo_state[2] -= (ev.data - axis_state[2]) * 5;
								}
							} else if (ev.data < -STICK_DEADZONE){
								if (ev.data - axis_state[2] < 0) {
									servo_state[2] -= (ev.data - axis_state[2]) * 5;
								}
							}

							servo_set(S3, constrain(servo_state[2], SERVO_S3_MIN_DUTY, SERVO_S3_MAX_DUTY));
							axis_state[2] = ev.data;
						}

						if (ev.part_id == 3) {
							if (ev.data > STICK_DEADZONE) {
								if (ev.data - axis_state[1] > 0) {
									servo_state[1] -= (ev.data - axis_state[1]) * 5;
								}
							} else if (ev.data < -STICK_DEADZONE){
								if (ev.data - axis_state[1] < 0) {
									servo_state[1] -= (ev.data - axis_state[1]) * 5;
								}
							}

							servo_set(S2, constrain(servo_state[1], SERVO_S2_MIN_DUTY, SERVO_S2_MAX_DUTY));
							axis_state[1] = ev.data;
						}

						if (ev.part_id == 18) {
							if (ev.data > STICK_DEADZONE) {
								if (ev.data - axis_state[0] > 0) {
									servo_state[0] -= (ev.data - axis_state[0]) * 5;
								}
							}

							servo_set(S1, constrain(servo_state[0], SERVO_S1_MIN_DUTY, SERVO_S1_MAX_DUTY));
							axis_state[0] = ev.data;
						}

						if (ev.part_id == 17) {
							if (ev.data > STICK_DEADZONE) {
								if (ev.data - axis_state[0] > 0) {
									servo_state[0] += (ev.data - axis_state[0]) * 5;
								}
							}

							servo_set(S1, constrain(servo_state[0], SERVO_S1_MIN_DUTY, SERVO_S1_MAX_DUTY));
							axis_state[0] = ev.data;
						}
						break;
					case LIBENJOY_EV_BUTTON:
						if ((ev.part_id == JOY_START) && (ev.data == 1)) {
							goto exit;
						}

						break;
					case LIBENJOY_EV_CONNECTED:
						printf("%u: status changed: %d\n", ev.joy_id, ev.data);
						break;
				}
			}

			usleep(50000);
			counter += 50;

			if(counter >= 1000)
			{
				libenjoy_enumerate(ctx);
				counter = 0;
			}
		}

		exit:
		libenjoy_close_joystick(joy);
	}

	libenjoy_free_info_list(info);
	libenjoy_close(ctx);

	destruct();

	deinit_devices();
	return 0;
}
