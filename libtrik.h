#ifndef _LIBTRIK_
#define _LIBTRIK_

#define ON "1"
#define OFF "0"

#define MOTOR_FREQ_M1 0x10
#define MOTOR_FREQ_M2 0x11
#define MOTOR_FREQ_M3 0x12
#define MOTOR_FREQ_M4 0x13

#define MOTOR_SPEED_M1 0x14
#define MOTOR_SPEED_M2 0x15
#define MOTOR_SPEED_M3 0x16
#define MOTOR_SPEED_M4 0x17

#define MOTOR_DEFAULT_PWM 0x1000
#define SERVO_DEFAULT_PERIOD "20000000"

enum servo {
	S1,
	S2,
	S3,
	S4,
	S5,
	S6
};

enum motor {
	M1,
	M2,
	M3,
	M4
};

void init_devices();
void deinit_devices();

int write_fd(const char* dev, char* value, int size);
int read_fd(const char* dev, char* buffer, int size);

char servo_enable(unsigned char num);
int servo_disable(int num);
int servo_set(int num, int val);

#endif
