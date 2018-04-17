#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libenjoy/src/libenjoy.h"
#include "libtrik.h"

#include <hgui.h>
#include "hgui/hguidemo/icon.xpm"

#define SERVO_S1_MIN_DUTY 1696325
#define SERVO_S1_MAX_DUTY 2262995

#define SERVO_S2_MIN_DUTY 700000
#define SERVO_S2_MAX_DUTY 2158665

#define SERVO_S3_MIN_DUTY 700000
#define SERVO_S3_MAX_DUTY 1895460

#define STICK_DEADZONE 500
#define BUTTON_DEADZONE 350

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

enum joy_axis {
	JOYA_LSTICK_X,
	JOYA_LSTICK_Y,
	JOYA_RSTICK_X,
	JOYA_RSTICK_Y,
	JOYA_GYROXL,
	JOYA_GYROY,
	JOYA_GYROXR,
	JOYA_UNKNOWN,
	JOYA_UP,
	JOYA_RIGHT,
	JOYA_DOWN,
	JOYA_LEFT,
	JOYA_L2,
	JOYA_R2,
	JOYA_L1,
	JOYA_R1,
	JOYA_TRIANGLE,
	JOYA_CIRCLE,
	JOYA_CROSS,
	JOYA_SQUARE
};

int mode = 0;
long axis_state[20] = { 0 };
int servo_state[6] = {
	1095460,
	1458665,
	1367695,
	0,
	0,
	0
};

char* STATUS_CONNECTED = "Joystick connected.";
char* STATUS_DISCONNECTED = "Joystick disconnected.";
char* STATUS_NOTFOUND = "Specified device not found.";
char* STATUS_NOINPUT = "No input devices found.";

hWindow *hw;
TextLine* status;
TextLine* joyStatus;

libenjoy_context *ctx;
libenjoy_joy_info_list *info;
libenjoy_joystick *joy;
int counter = 0;
signed char found = -128;

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

void loop() {
	libenjoy_event ev;

	while(libenjoy_poll(ctx, &ev))
	{
		switch(ev.type)
		{
			case LIBENJOY_EV_AXIS:
				if (mode == 0) {
					if (ev.part_id == JOYA_LSTICK_Y) {
						long diff = (ev.data - axis_state[JOYA_LSTICK_Y]);

						if (ev.data > STICK_DEADZONE) {
							if (diff > 0) {
								servo_state[S3] -= diff * 5;
							}
						} else if (ev.data < -STICK_DEADZONE){
							if (diff < 0) {
								servo_state[S3] -= diff * 5;
							}
						}

						servo_state[S3] = constrain(servo_state[S3], SERVO_S3_MIN_DUTY, SERVO_S3_MAX_DUTY);

						servo_set(S3, servo_state[S3]);
						axis_state[JOYA_LSTICK_Y] = ev.data;
					}

					if (ev.part_id == JOYA_RSTICK_Y) {
						long diff = (ev.data - axis_state[JOYA_RSTICK_Y]);

						if (ev.data > STICK_DEADZONE) {
							if (diff > 0) {
								servo_state[S2] -= diff * 5;
							}
						} else if (ev.data < -STICK_DEADZONE){
							if (diff < 0) {
								servo_state[S2] -= diff * 5;
							}
						}

						servo_state[S2] = constrain(servo_state[S2], SERVO_S2_MIN_DUTY, SERVO_S2_MAX_DUTY);

						servo_set(S2, servo_state[S2]);
						axis_state[JOYA_RSTICK_Y] = ev.data;
					}
				}

				if (mode == 1) {
					if (ev.part_id == JOYA_GYROY) {
						long diff = (ev.data - axis_state[JOYA_GYROY]);

						if (ev.data > STICK_DEADZONE) {
							if (diff > 0) {
								servo_state[S3] -= diff * 5;
								servo_state[S2] += diff * 5;
							}
						} else if (ev.data < -STICK_DEADZONE){
							if (diff < 0) {
								servo_state[S3] -= diff * 5;
								servo_state[S2] += diff * 5;
							}
						}

						servo_state[S3] = constrain(servo_state[S3], SERVO_S3_MIN_DUTY, SERVO_S3_MAX_DUTY);
						servo_state[S2] = constrain(servo_state[S2], SERVO_S2_MIN_DUTY, SERVO_S2_MAX_DUTY);

						servo_set(S3, servo_state[S3]);
						servo_set(S2, servo_state[S2]);
						axis_state[JOYA_GYROY] = ev.data;
					}
				}

				if (mode == 2) {

				}

				// Clamp
				if (ev.part_id == JOYA_CROSS) {
					if (ev.data > BUTTON_DEADZONE) {

						long diff = ev.data - axis_state[JOYA_CROSS];
						if (diff > 0) {
							servo_state[S1] -= diff * 5;
						}
					}

					servo_state[S1] = constrain(servo_state[S1], SERVO_S1_MIN_DUTY, SERVO_S1_MAX_DUTY);

					servo_set(S1, servo_state[S1]);
					axis_state[JOYA_CROSS] = ev.data;
				}

				if (ev.part_id == JOYA_CIRCLE) {
					if (ev.data > BUTTON_DEADZONE) {
						long diff = ev.data - axis_state[JOYA_CIRCLE];

						if (diff > 0) {
							servo_state[S1] += diff * 5;
						}
					}

					servo_state[S1] = constrain(servo_state[S1], SERVO_S1_MIN_DUTY, SERVO_S1_MAX_DUTY);

					servo_set(S1, servo_state[S1]);
					axis_state[JOYA_CIRCLE] = ev.data;
				}

				// Wheels
				// if (ev.part_id == JOYA_UP) {
				// 	if (ev.data > BUTTON_DEADZONE) {
				// 			long diff = constrain(map(ev.data, BUTTON_DEADZONE, 32767, 10, 70), 0, 70);
				// 			printf("Up axis: %d\n", diff);
				//
				// 			if (ev.data - axis_state[JOYA_UP] > 1500) {
				// 				motor_set(M1, diff);
				// 				motor_set(M2, diff);
				// 				motor_set(M3, diff);
				// 				motor_set(M4, diff);
				// 			}
				// 	}
				//
				// 	axis_state[JOYA_UP] = ev.data;
				// }
				//
				// if (ev.part_id == JOYA_DOWN) {
				// 	if (ev.data > BUTTON_DEADZONE) {
				// 		long diff = constrain(map(ev.data, BUTTON_DEADZONE, 32767, 10, 70), 0, 70);
				//
				// 		if (diff > 0) {
				// 			motor_set(M1, -diff);
				// 			motor_set(M2, -diff);
				// 			motor_set(M3, -diff);
				// 			motor_set(M4, -diff);
				// 		}
				// 	}
				//
				// 	axis_state[JOYA_DOWN] = ev.data;
				// }
				//
				// if (ev.part_id == JOYA_LEFT) {
				// 	if (ev.data > BUTTON_DEADZONE) {
				// 		long diff = constrain(map(ev.data, BUTTON_DEADZONE, 32767, 10, 70), 0, 70);
				//
				// 		if (diff > 0) {
				// 			motor_set(M1, diff);
				// 			motor_set(M2, diff);
				// 			motor_set(M3, -diff);
				// 			motor_set(M4, -diff);
				// 		}
				// 	}
				//
				// 	axis_state[JOYA_LEFT] = ev.data;
				// }
				//
				// if (ev.part_id == JOYA_RIGHT) {
				// 	if (ev.data > BUTTON_DEADZONE) {
				// 		long diff = constrain(map(ev.data, BUTTON_DEADZONE, 32767, 10, 70), 0, 70);
				//
				// 		if (diff > 0) {
				// 			motor_set(M1, -diff);
				// 			motor_set(M2, -diff);
				// 			motor_set(M3, diff);
				// 			motor_set(M4, diff);
				// 		}
				// 	}
				//
				// 	axis_state[JOYA_RIGHT] = ev.data;
				// }
				//
				// if (ev.part_id == JOYA_L1) {
				// 	if (ev.data > BUTTON_DEADZONE) {
				// 		long diff = constrain(map(ev.data, BUTTON_DEADZONE, 32767, 10, 70), 0, 70);
				//
				// 		if (diff > 0) {
				// 			motor_set(M1, diff);
				// 			motor_set(M2, -diff);
				// 			motor_set(M3, diff);
				// 			motor_set(M4, -diff);
				// 		}
				// 	}
				//
				// 	axis_state[JOYA_L1] = ev.data;
				// }
				//
				// if (ev.part_id == JOYA_R1) {
				// 	if (ev.data > BUTTON_DEADZONE) {
				// 		long diff = constrain(map(ev.data, BUTTON_DEADZONE, 32767, 10, 70), 0, 70);
				//
				// 		if (diff > 0) {
				// 			motor_set(M1, -diff);
				// 			motor_set(M2, diff);
				// 			motor_set(M3, -diff);
				// 			motor_set(M4, diff);
				// 		}
				// 	}
				//
				// 	axis_state[JOYA_R1] = ev.data;
				// }
				break;
			case LIBENJOY_EV_BUTTON:
				if ((ev.part_id == JOY_START) && (ev.data == 1)) {

				}

				if ((ev.part_id == JOY_SELECT) && (ev.data == 1)) {
					mode = (mode + 1) % 3;
				}

				// if (((ev.part_id == JOY_UP) || (ev.part_id == JOY_DOWN) ||
				// 	(ev.part_id == JOY_LEFT) || (ev.part_id == JOY_RIGHT) ||
				// 	(ev.part_id == JOY_L1) || (ev.part_id == JOY_R1))
				// && (ev.data == 0)) {
				// 	printf("Button: %d %d\n", ev.part_id, ev.data);
				// 	motor_set(M1, -10);
				// 	motor_set(M2, -20);
				// 	motor_set(M3, -10);
				// 	motor_set(M4, -10);
				// 	usleep(100);
				// 	motor_set(M1, 0);
				// 	motor_set(M2, 0);
				// 	motor_set(M3, 0);
				// 	motor_set(M4, 0);
				// }

				break;
			case LIBENJOY_EV_CONNECTED:
				//clearWindow(hw);
				if (ev.data == 0) {
					changeTextPtr(joyStatus, STATUS_DISCONNECTED);
				} else {
					changeTextPtr(joyStatus, STATUS_CONNECTED);
				}
				redrawWindow(hw);
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

int main(int argc, char const *argv[]) {
	init_devices();

	hw = createWindow(0,0,320,240,"TRIK Manipulator",0x00B653,0x000000,320,240,0,icon_xpm,NULL);
	hideMouseCursor(hw);
	status = addTextLine(hw,0,0,"Initialized.",0x000000,"-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*");
	joyStatus = addTextLine(hw,0,16,"Waiting for joystick...",0x000000,"-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*");

	ctx = libenjoy_init();

	reenumerate:
	libenjoy_enumerate(ctx);

	info = libenjoy_get_info_list(ctx);

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
			redrawWindow(hw);
			sleep(1);
			goto reenumerate;
		} else {
			joy = libenjoy_open_joystick(ctx, info->list[found]->id);

			if (joy) {
				printf("Connected to %d\n", info->list[found]->id);
				changeTextPtr(joyStatus, STATUS_CONNECTED);
				redrawWindow(hw);

				setup();

				eventLoop(hw, &loop, NULL);

				libenjoy_close_joystick(joy);
			}
		}
	} else {
		changeTextPtr(joyStatus, STATUS_NOINPUT);

		fprintf(stderr, "No input devices found.\n");
		eventLoop(hw, NULL, NULL);
	}

	libenjoy_free_info_list(info);
	libenjoy_close(ctx);

	destruct();
	deinit_devices();

	destroyWindow(hw);
	return 0;
}
