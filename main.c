/* Scheduler includes. */
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
#include "shell.h"

volatile xSemaphoreHandle serial_tx_wait_sem = NULL;

volatile xQueueHandle serial_rx_queue = NULL;

typedef struct {
	char ch;
}serial_ch_msg;

/*IRQ handler to handle USART2 interrupts(both transmit and receive interrupts)*/
void USART3_IRQHandler(){
	static signed portBASE_TYPE xHigherPriorityTaskWoken;
	serial_ch_msg rx_msg;

	/*If this interrupt is for a transmit...*/
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET){
		/*"give" the serial_tx_wait_sem semaphore to notify processes
		 *that the buffer has a spot free for the next byte.
		 */
		xSemaphoreGiveFromISR(serial_tx_wait_sem, &xHigherPriorityTaskWoken);
		/*Disable the transmit interrupt.*/
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	/*If this interrupt is for a receive...*/
	}else if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		/*Receive the byte from the buffer.*/
		rx_msg.ch = USART_ReceiveData(USART3);

		/*Queue the received byte.*/
		if(!xQueueSendToBackFromISR(serial_rx_queue, &rx_msg, &xHigherPriorityTaskWoken)){
			/*If there was an error queueing the received byte, freeze!*/
			while(1);
		}
	/*Only transmit and receive interrupts should be enabled.
	 *If this is another type of interrupt, freeze.*/
	}else{
		while(1);
	}

	if(xHigherPriorityTaskWoken){
		taskYIELD();
	}
}

void send_byte(char ch){
	/*Wait until the RS232 port can receive another byte
	 *(this semaphore is "given" by the RS232 port interrupt
	 *when the buffer has room for another byte.)
	 */
	while (!xSemaphoreTake(serial_tx_wait_sem, portMAX_DELAY));
	/*Send the byte and enable the transmit interrupt
	 *(it is diabled by the interrupt).
	 */
	USART_SendData(USART3, ch);
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

char receive_byte(){
	serial_ch_msg msg;
	/*Wait for a byte to be queued by the receive interrupts handler.*/
	while(!xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY));
	return msg.ch;
}

char non_block_receive_byte(){
	char ch;
	char str[2];
	ch = USART_RecvByte(USART3);
	//Delay_100ms(10);	
	//USART_SendData(USART3, (uc8*)&ch);
	//sendch(&ch);
	/*
	str[0] = ch;
	str[1] = '\0';
	sendstr(str);
	*/
	//sendstr("123");
	return ch;
	/*
	serial_ch_msg msg;
	
	int ret = xQueueReceive(serial_rx_queue, &msg, portMAX_DELAY);
	if(ret == pdTRUE){
		return msg.ch;
	}*/
}

void sendstr(char *str){
	RS232_SendStr(USART3, str);
}
/*
void recvstr(){
	RS232_RecvStr(USART3, );
}
*/
int main( void )
{
  //u8 Sta = ERROR;
  //FSM_Mode FSM_State = FSM_Rx;

	/* System Init */
	System_Init();
	/*char ch = 'a';
	while(1){
		USART_SendByte(USART3, ch);
		ch = USART_RecvByte(USART3);
		USART_SendByte(USART3, ch);
	}*/
	/*Create the queue used by the serial task.
	 *Messages for write to the RS232*/
	vSemaphoreCreateBinary(serial_tx_wait_sem);
	serial_rx_queue = xQueueCreate(1, sizeof(serial_ch_msg));

	/*Create a task*/
	xTaskCreate(user_shell,
			   (signed portCHAR *) "User Shell",
			   512 /*stack size*/, NULL, tskIDLE_PRIORITY + 2, NULL);

	/*Start running the tasks.*/
	vTaskStartScheduler();

  /* Lock */
  //LED_R = 1;
  //LED_G = 0;
  //LED_B = 1;

  /* Final State Machine */
	//while(1){
	//	RS232_SendStr(USART3, "hello");
//	LED_G = ~LED_G; 
//	LED_B = ~LED_B;
//	LED_R = ~LED_R;

//	Delay_100ms(10);	

	//}
	return 0;
}
/*=====================================================================================================*/
/*=====================================================================================================*/
