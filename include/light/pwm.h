#ifndef PWM_H
#define PWM_H

enum pwm_mode {
	NORMAL,
	INVERTED
};

int init_pwm(int pwm_id);
int destroy_pwm(int pwm_id);
int set_pwm_period(int pwm_id, int period);
int set_pwm_duty(int pwm_id, int duty);
void start_pwm(int pwm_id);
void stop_pwm(int pwm_id);

#endif
