#include "pwm.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>



int init_pwm(int pwm_id)
{
	int fd, len;
	char buffer[128];

	fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
	if (fd < 0) {
		perror("Failed open pwm export.");
		return -1;
	}

	len = snprintf(buffer, sizeof(buffer), "%d", pwm_id);
	if (write(fd, buffer, len) < 0) {
		close(fd);
		perror("Falied to export pwm.");
		return -1;
	}

	close(fd);
	usleep(500000);
	return 0;
}

int destroy_pwm(int pwm_id)
{
	int fd, len;
	char buffer[128];

	fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
	if (fd < 0) {
		return -1;
	}

	len = snprintf(buffer, sizeof(buffer), "%d", pwm_id);
	if (write(fd, buffer, len) < 0) {
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int set_pwm_period(int pwm_id, int period)
{
	int fd, len;
	char path[128];
	char period_str[512];
	snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/period", pwm_id);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror("Failed to open pwm_polarity for write.");
		return -1;
	}
	
	len = snprintf(period_str, sizeof(period_str), "%d", period);

	if (write(fd, period_str, len) < 0)
		perror("Failed to write pwm_polarity");
	
	return 0;
}

int set_pwm_duty(int pwm_id, int duty)
{

	int fd, len;
	char path[128];
	char period_str[512];
	snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/duty_cycle", pwm_id);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror("Failed to open pwm_duty for write.");
		return -1;
	}
	
	len = snprintf(period_str, sizeof(period_str), "%d", duty);

	if (write(fd, period_str, len) < 0)
		perror("Failed to write pwm_duty");
	
	return 0;
}

void start_pwm(int pwm_id)
{
	int fd;
	char path[128];
	char period_str[512];
	snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/enable", pwm_id);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror("Failed to open pwm_duty for write.");
		return;
	}

	write(fd, "1", 1);
}

void stop_pwm(int pwm_id)
{
	int fd;
	char path[128];
	char period_str[512];
	snprintf(path, sizeof(path), "/sys/class/pwm/pwmchip0/pwm%d/enable", pwm_id);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror("Failed to open pwm_duty for write.");
		return;
	}

	write(fd, "0", 1);
}

