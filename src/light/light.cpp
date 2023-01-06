#include "pwm.h"
#include "light.h"


int lightInit(int light_id)
{
	int ret = 0;

	destroy_pwm(light_id);
	ret = init_pwm(light_id);
	if(ret < 0) {
		return -1;
	}
	ret = set_pwm_period(light_id, 1000000);
	if(ret < 0)
		return -1;
	ret = set_pwm_duty(light_id, 0);
	if(ret < 0)
		return -1;
	
	start_pwm(light_id);
	return 0;
}

void lightSet(int light_id, int lightNess)
{
	set_pwm_duty(light_id, lightNess * 10000);
}


