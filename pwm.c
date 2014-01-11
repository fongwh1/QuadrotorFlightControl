#include "module_motor.h"
#include "stm32f4xx.h"
#include "stm32f4_delay.h"
#include "string-util.h"
#include "pwm.h"
#include "transport.h"

extern char status;
extern int status_mod_num;

void PWM_Config( void ){
	Motor_Config();
}

void PWM_print()
{
        printf("1: %d 2: %d 3: %d 4: %d\n",PWM_Motor1,PWM_Motor2,PWM_Motor3,PWM_Motor4);
}

void PWM_all_motor_pulse(int pulse){

	if(pulse <= 1900 && pulse > 700)
	{
		PWM_Motor1 = pulse;
		Delay_100us(5);
		PWM_Motor2 = pulse;
		Delay_100us(5);
		PWM_Motor3 = pulse;
		Delay_100us(5);
		PWM_Motor4 = pulse;
		Delay_100us(5);
	}

}

void PWM_run(){
	int pwm_pulse = PWM_Motor1;
		while(pwm_pulse <= 1500)
		{
			pwm_pulse += 20;
			PWM_all_motor_pulse(pwm_pulse);
			PWM_print();
		}
/*
	int PWM_pulse = 1600;
	if(PWM_pulse <= 1600){
		PWM_pulse += 100;
		
		Delay_10ms(10);
	}
*/
}

void PWM_stop(){
	int pwm_pulse = PWM_Motor1;

		while(pwm_pulse > 800)
		{
			pwm_pulse -= 15;
			PWM_all_motor_pulse(pwm_pulse);
			PWM_print();
		}
/*
	int PWM_pulse = PWM_Motor1;
	while(PWM_pulse > 0){
		PWM_pulse -= 200;
		PWM_Motor1 = PWM_pulse;
  		PWM_Motor2 = PWM_pulse;
  		PWM_Motor3 = PWM_pulse;
  		PWM_Motor4 = PWM_pulse;
		Delay_10ms(5);
	}
*/
}

void PWM_task(void * pvParameters)
{

//	int pwm_pulse = 1300;

	Delay_100ms(20);
	printf("PWM runs in 10 sec\n");
	Delay_100ms(20);

/*
		while(pwm_pulse <= 1900)
		{
			pwm_pulse += 10;
			PWM_all_motor_pulse(pwm_pulse);
			PWM_print();
		}

		while(pwm_pulse > 800)
		{
			pwm_pulse -= 15;
			PWM_all_motor_pulse(pwm_pulse);
			PWM_print();
		}

		Delay_1ms(50);
*/

	int expected_pwm = 0;
	int pwm_pulse;
	status = STOP;
	while(1){
		pwm_pulse = PWM_Motor1;
		if(status == STOP && pwm_pulse > 800){
			//PWM_stop();
			pwm_pulse -= 15;
			PWM_all_motor_pulse(pwm_pulse);
			PWM_print();
		}else if(status == START && pwm_pulse <= 1500){
			pwm_pulse += 20;
			PWM_all_motor_pulse(pwm_pulse);
			PWM_print();
			//PWM_start();
		}else if(status == MOD){
			expected_pwm = pwm_pulse + status_mod_num;

			while(pwm_pulse != expected_pwm)
			{
				if (pwm_pulse > expected_pwm)
				{

					pwm_pulse -=10;

				}else{
					pwm_pulse +=10;

				}
				PWM_all_motor_pulse(pwm_pulse);
				PWM_print();
			}
			status = IDLE;
		}
	}
}
