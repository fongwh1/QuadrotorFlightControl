#include "module/module_nrf24l01.h"
#include "module_motor.h"
#include "string-util.h"
#include "pwm.h"
#include "sensor.h"
#include "module_sensor.h"
#include "transport.h"

#define BUF_SIZE 50

char rec_word[BUF_SIZE];
u8 status;
int status_mod_num;

void transport(char* send_str){
	u8 Sta;
	nRF_TX_Mode();
	printf("Send:%s\n", send_str);
	Sta = nRF_Tx_Nonblock_Data(send_str);
	if(Sta == TX_DS){
		printf("Success\n");
	}else{
		printf("Failure\n");
	}
}

int string_to_num(char *str){
	int digit = 0;
	int sum = 0;
	int i = 0;
	while(1){
		if(*(str+i) == '\0')break;
		digit++;
		i++;
	}
	for(i = 0; i < digit; i++){
		sum *= 10;
		sum += *(str+i)-48;
	}
	return sum;
}

void check_status(){
	char cmp_word[BUF_SIZE];
	char send_str[BUF_SIZE];
	
	strcpy(cmp_word, rec_word);

	//cmp_word[strlen(cmp_word) - 1] = '\0';
	
	if(strcmp(cmp_word, "stop") == 0){
		printf("motor stop!!\n");
		status = STOP;
		//PWM_stop();
	}else if(strcmp(cmp_word, "start") == 0){
		printf("motor start!!\n");
		status = START;
		//PWM_run();
	}else if(strcmp(cmp_word, "pwm") == 0){
		printf("send pwm\n");
		sprintf(send_str, "1:%d 2:%d 3:%d 4:%d\n", PWM_Motor1, PWM_Motor2, PWM_Motor3, PWM_Motor4);
		transport(send_str);
	}else if(strcmp(cmp_word, "sensor") == 0){
		printf("send sensor\n");
		sprintf(send_str, "Acc: %f, %f, %f\n", Acc.TrueX, Acc.TrueY, Acc.TrueZ);
		transport(send_str);
	}else if(cmp_word[0] == 'u'){
		status_mod_num = string_to_num(cmp_word + 1);
		printf("motor add %d\n", status_mod_num);
		status = MOD;
	}else if(cmp_word[0] == 'd'){
		status_mod_num = string_to_num(cmp_word + 1) * (-1);
		printf("motor add %d\n", status_mod_num);
		status = MOD;
	}
}

void init_string(){
	int i;
	for(i = 0; i < BUF_SIZE; i++){
		rec_word[i] = '\0';
	}
}

void transport_task(){
	u8 Sta;
	int i = 0;
	while(1){
		init_string();
		nRF_RX_Mode();
		Sta = nRF_Rx_Nonblock_Data(rec_word);
		if(Sta == RX_DR){
			printf("I've received %s\n", rec_word);
			check_status();
			i = 0;
		}else{
			printf("Error\n");
			i++;
		}
		if(i >= 3){
			printf("Connection failure!!\n");
			status = STOP;
		}
	}
}
