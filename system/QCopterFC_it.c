/*=====================================================================================================*/
/*=====================================================================================================*/
#include "stm32f4_system.h"
#include "stm32f4_i2c.h"
#include "QCopterFC.h"
#include "QCopterFC_ctrl.h"
#include "QCopterFC_ahrs.h"
#include "QCopterFC_transport.h"
#include "module_board.h"
#include "module_motor.h"
#include "module_sensor.h"
#include "module_mpu9150.h"
#include "module_ms5611.h"
#include "algorithm_pid.h"
#include "algorithm_moveAve.h"
#include "algorithm_mathUnit.h"
#include "algorithm_quaternion.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
vu8 Time_Sec = 0;
vu8 Time_Min = 0;
vu8 RecvTime_Sec = 0;
vu8 RecvTime_Min = 0;
vu16 SysTick_Cnt = 0;

vs16 Tmp_PID_KP;
vs16 Tmp_PID_KI;
vs16 Tmp_PID_KD;
vs16 Tmp_PID_Pitch;

Sensor_Mode SensorMode = Mode_GyrCorrect;
/*=====================================================================================================*/
/*=====================================================================================================*/



/*=====================================================================================================*/
/*=====================================================================================================*/
void DMA1_Stream0_IRQHandler( void )
{
	I2C_RX_DMA_IRQ();
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void DMA1_Stream6_IRQHandler( void )
{
	I2C_TX_DMA_IRQ();
}
/*=====================================================================================================*/
/*=====================================================================================================*/
void HardFault_Handler( void )
{
  while(1);
}
void MemManage_Handler( void )
{
  while(1);
}
void BusFault_Handler( void )
{
  while(1);
}
void UsageFault_Handler( void )
{
  while(1);
}
/*void SVC_Handler( void )
{
  while(1);
}*/
void DebugMon_Handler( void )
{
  while(1);
}
/*void PendSV_Handler( void )
{
  while(1);
}*/
void NMI_Handler( void )
{
  while(1);
}
/*=====================================================================================================*/
/*=====================================================================================================*/
