#ifndef USARTIO_H
#define USARTIO_H

void USARTIO_init();
void USART_IRQHandler();
void send_byte(char ch);
char receive_byte();
char non_block_receive_byte();
void send_str(char *str);

#endif
