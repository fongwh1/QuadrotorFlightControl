#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4_system.h"
#include "stm32f4_delay.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_rs232.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_nrf24l01.h"

/* Shell includes */
#include "usartIO.h"
#include "sensor.h"
#include "pwm.h"
#include "transport.h"

int main( void )
{
	/* System Init */
	System_Init();
	
	USARTIO_Init();

	LED_G = (Sensor_Init() == SUCCESS) ? 0 : 1;

	/*Create a task*/
	xTaskCreate(sensor_task,
			   (signed portCHAR *) "Collecting data from sensors",
			   512, NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(PWM_task,
			   (signed portCHAR *) "Assign value to PWM",
			   512, NULL, tskIDLE_PRIORITY + 2, NULL);
/*
	xTaskCreate(user_shell,
			   (signed portCHAR *) "User Shell",
*///			   512 /*stack size*/, NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(transport_task,
			   (signed portCHAR *) "Receive and send value",
			   512, NULL, tskIDLE_PRIORITY + 2, NULL);

	/*Start running the tasks.*/
	vTaskStartScheduler();

	return 0;
}
