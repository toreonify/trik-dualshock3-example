#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "libtrik.h"

#define _DEBUG 1

const char* LED_RED_ENABLE = "/sys/devices/platform/leds-gpio/leds/led_red/brightness";
const char* LED_GREEN_ENABLE = "/sys/devices/platform/leds-gpio/leds/led_green/brightness";

const char* SERVO[] = {
	"/sys/class/pwm/ecap.0/%s",
	"/sys/class/pwm/ecap.1/%s",
	"/sys/class/pwm/ecap.2/%s",
	"/sys/class/pwm/ehrpwm.0:1/%s",
	"/sys/class/pwm/ehrpwm.1:0/%s",
	"/sys/class/pwm/ehrpwm.1:1/%s"
};
#define SERVO_SIZE sizeof(SERVO) / sizeof(const char*)

const char* MOTOR_ENABLE = "/sys/class/gpio/gpio62/value";
const char* MOTOR_I2C_COMMAND = "i2cset -y 2 0x48 0x%X 0x%X w";

#define WRITE_BUF_SIZE 1024
#define TEMP_BUF_SIZE 32

void init_devices() {
	char* buf = malloc(WRITE_BUF_SIZE + 1);
	char* tmp = malloc(TEMP_BUF_SIZE + 1);

	write_fd(LED_GREEN_ENABLE, OFF, sizeof(OFF));
	write_fd(LED_RED_ENABLE, OFF, sizeof(OFF));

	write_fd(MOTOR_ENABLE, ON, sizeof(ON));

	char i;
	for (i = 0; i < SERVO_SIZE; i++) {
		snprintf(buf, WRITE_BUF_SIZE, SERVO[i], "request");
		if (read_fd(buf, tmp, TEMP_BUF_SIZE) == 0) {
			write_fd(buf, ON, sizeof(ON));
		}
	}

	snprintf(buf, WRITE_BUF_SIZE, MOTOR_I2C_COMMAND, MOTOR_FREQ_M1, MOTOR_DEFAULT_PWM);
	system(buf);

	snprintf(buf, WRITE_BUF_SIZE, MOTOR_I2C_COMMAND, MOTOR_FREQ_M2, MOTOR_DEFAULT_PWM);
	system(buf);

	snprintf(buf, WRITE_BUF_SIZE, MOTOR_I2C_COMMAND, MOTOR_FREQ_M3, MOTOR_DEFAULT_PWM);
	system(buf);

	snprintf(buf, WRITE_BUF_SIZE, MOTOR_I2C_COMMAND, MOTOR_FREQ_M4, MOTOR_DEFAULT_PWM);
	system(buf);

	free(buf);
	free(tmp);
}

void deinit_devices() {
	write_fd(LED_GREEN_ENABLE, OFF, sizeof(OFF));
	write_fd(LED_RED_ENABLE, OFF, sizeof(OFF));

	write_fd(MOTOR_ENABLE, OFF, sizeof(OFF));
}

int write_fd(const char* dev, char* value, int size) {
	int fd = open(dev, O_WRONLY);

	if (fd < 0) {
#ifdef _DEBUG
		fprintf(stderr, "[%s]Cannot open %s\n", __func__, dev);
#endif
		return fd;
	}

	if (write(fd, value, size) < 0) {
#ifdef _DEBUG
		fprintf(stderr, "[%s]Cannot write %s\n", __func__, dev);
#endif
		return -1;
	}

#ifdef _DEBUG
	printf("[%s]Writing %s to %s\n", __func__, value, dev);
#endif

	close(fd);
	return 0;
}

int read_fd(const char* dev, char* buffer, int size) {
	int fd = open(dev, O_RDONLY);

	if (fd < 0) {
#ifdef _DEBUG
		fprintf(stderr, "[%s]Cannot open %s\n", __func__, dev);
#endif
		return fd;
	}

	if (read(fd, buffer, size) < 0) {
#ifdef _DEBUG
		fprintf(stderr, "[%s]Cannot read %s\n", __func__, dev);
#endif
		return -1;
	}

#ifdef _DEBUG
		printf("[%s]Reading %s from %s\n", __func__, buffer, dev);
#endif

	close(fd);
	return 0;
}

char servo_enable(unsigned char num) {
	if (num < SERVO_SIZE) {
		char* buf = malloc(WRITE_BUF_SIZE + 1);

		snprintf(buf, WRITE_BUF_SIZE, SERVO[num], "period_ns");
		write_fd(buf, SERVO_DEFAULT_PERIOD, sizeof(SERVO_DEFAULT_PERIOD));

		snprintf(buf, WRITE_BUF_SIZE, SERVO[num], "run");
		write_fd(buf, ON, sizeof(ON));

		free(buf);
		return 0;
	}

	return -1;
}

int servo_disable(int num) {
	if (num < SERVO_SIZE) {
		char* buf = malloc(WRITE_BUF_SIZE + 1);

		snprintf(buf, WRITE_BUF_SIZE, SERVO[num], "run");
		write_fd(buf, OFF, sizeof(OFF));

		free(buf);
		return 0;
	}

	return -1;
}

int servo_set(int num, int val) {
	if (num < SERVO_SIZE) {
		char* buf = malloc(WRITE_BUF_SIZE + 1);
		char* vbuf = malloc(TEMP_BUF_SIZE + 1);

		snprintf(buf, WRITE_BUF_SIZE, SERVO[num], "duty_ns");
		snprintf(vbuf, TEMP_BUF_SIZE, "%d", val);
		write_fd(buf, vbuf, TEMP_BUF_SIZE);

		snprintf(buf, WRITE_BUF_SIZE, SERVO[num], "run");
		write_fd(buf, ON, sizeof(ON));

		free(buf);
		free(vbuf);
		return 0;
	}

	return -1;
}
