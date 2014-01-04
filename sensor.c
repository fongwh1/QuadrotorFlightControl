#include "stm32f4_system.h"
#include "stm32f4_i2c.h"
#include "stm32f4_delay.h"

#include "module_board.h" // for the LED
#include "module_sensor.h"
#include "module_ak8975.h"
#include "module_mpu6050.h"
#include "module_mpu9150.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sensor.h"

extern SensorAcc Acc;
extern SensorGyr Gyr;
extern SensorMag Mag;
extern SensorTemp Temp;

void sensor_task(void * pvParameters)
{

	u8 IMU_Buf[20] = {0};
	
	while(1)
	{
		LED_R = ~LED_R;
		MPU9150_Read(IMU_Buf);

		Acc.X  = (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
  		Acc.Y  = (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
  		Acc.Z  = (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
  		Temp.T = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
  		Gyr.X  = (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
  		Gyr.Y  = (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
  		Gyr.Z  = (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);
  		Mag.X  = (s16)((IMU_Buf[15] << 8) | IMU_Buf[14]);
  		Mag.Y  = (s16)((IMU_Buf[17] << 8) | IMU_Buf[16]);
  		Mag.Z  = (s16)((IMU_Buf[19] << 8) | IMU_Buf[18]);

/*	
		data_buf[ACCE][X]  = (s16)(Acc.TrueX*1000);  // 1 mg/LSB
		data_buf[ACCE][Y]  = (s16)(Acc.TrueY*1000);  // 1 mg/LSB
    		data_buf[ACCE][Z]  = (s16)(Acc.TrueZ*1000);  // 1 mg/LSB
    		data_buf[GYRO][X]  = (s16)(Gyr.TrueX*100);   // 10 mdps/LSB
    		data_buf[GYRO][Y]  = (s16)(Gyr.TrueY*100);   // 10 mdps/LSB
    		data_buf[GYRO][Z]  = (s16)(Gyr.TrueZ*100);   // 10 mdps/LSB
    		data_buf[MAG][X]  = (s16)(Mag.TrueX);       // 100 nTesla/LSB
    		data_buf[MAG][Y]  = (s16)(Mag.TrueY);       // 100 nTesla/LSB
    		data_buf[MAG][Z]  = (s16)(Mag.TrueZ);       // 100 nTesla/LSB
    		data_buf[TEMPATURE][0]  = (s16)(Temp.TrueT*100);  // 0.01 degC/LSB
*/
		Delay_10ms(100);
	
		printf("%d,%d,%d\n", Acc.X,Acc.Y,Acc.Z);
	
	}

}
