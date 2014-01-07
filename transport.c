#include "module/module_nrf24l01.h"
#include "string-util.h"
#include "pwm.h"

char rec_word[10];

void check_status(){
	char cmp_word[10];
	
	strcpy(cmp_word, rec_word);

	cmp_word[strlen(cmp_word) - 1] = '\0';
	
	if(strcmp(cmp_word, "stop") == 0){
		printf("motor stop!!\n");
		PWM_stop();
	}else if(strcmp(cmp_word, "start") == 0){
		printf("motor start!!\n");
		PWM_run();
	}
}

void init_string(){
	int i;
	for(i = 0; i < 10; i++){
		rec_word[i] = '\0';
	}
}

void transport_task(){
	u8 Sta;
	while(1){
		init_string();
		nRF_RX_Mode();
		Sta = nRF_Rx_Nonblock_Data(rec_word);
		if(Sta == RX_DR){
			printf("I've received %s\n", rec_word);
			check_status();
		}else{
			printf("Error\n");
		}
	}
}
