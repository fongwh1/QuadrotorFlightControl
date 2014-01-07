#include "stm32f4_system.h"
#include "stm32f4_i2c.h"
#include "stm32f4_delay.h"

#include "module_board.h" // for the LED
#include "module_sensor.h"
#include "module_ak8975.h"
#include "module_mpu6050.h"
#include "module_mpu9150.h"

#include "algorithm_moveAve.h" // for function MoveAve_SMA/MoveAve_WMA

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sensor.h"
#include <math.h>

#define	SAMPLING_TIMES	2000

extern SensorAcc Acc;
extern SensorGyr Gyr;
extern SensorMag Mag;
extern SensorTemp Temp;

u8 IMU_Buf[20] = {0};

void test(void)
{
	int a = 1, b = 0;
	float c = 0;

	c = atan2(a,b);

	printf("%f,  %f,  %f,   %f,   %f\n",0.1,0.01,0.001,0.0001,0.00001);

}

void sensor_offset_sampling(void)
{
	int sampling_times = SAMPLING_TIMES;

	Delay_10ms(100);
	Acc.OffsetX = 0;
	Acc.OffsetY = 0;
	Acc.OffsetX = 0;

	while(sampling_times > 0)
	{
		MPU9150_Read(IMU_Buf);
		LED_B = ~LED_B;

                Acc.OffsetX  += (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
                Acc.OffsetY  += (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
                Acc.OffsetZ  += (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
                Temp.T = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
                Gyr.OffsetX  += (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
                Gyr.OffsetY  += (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
                Gyr.OffsetZ  += (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);
                Mag.X  = (s16)((IMU_Buf[15] << 8) | IMU_Buf[14]);
                Mag.Y  = (s16)((IMU_Buf[17] << 8) | IMU_Buf[16]);
                Mag.Z  = (s16)((IMU_Buf[19] << 8) | IMU_Buf[18]);
	
		sampling_times--;
	}

	Acc.OffsetX /= SAMPLING_TIMES;
	Acc.OffsetY /= SAMPLING_TIMES;
	Acc.OffsetZ /= SAMPLING_TIMES;

	Gyr.OffsetX /= SAMPLING_TIMES;
	Gyr.OffsetY /= SAMPLING_TIMES;
	Gyr.OffsetZ /= SAMPLING_TIMES;
	
	LED_B = 0;
	Delay_10ms(100);
	LED_B = 1;
}

void sensor_task(void * pvParameters)
{

	sensor_offset_sampling();

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


		Acc.TrueX  = LOW_PASS(Acc.TrueX,(Acc.X - Acc.OffsetX));
		Acc.TrueY  = LOW_PASS(Acc.TrueY,(Acc.Y - Acc.OffsetY));
		Acc.TrueZ  = LOW_PASS(Acc.TrueZ,(Acc.Z - Acc.OffsetZ));
		Gyr.X  -= Gyr.OffsetX;
		Gyr.Y  -= Gyr.OffsetY;
		Gyr.Z  -= Gyr.OffsetZ;
		Mag.X  *= Mag.AdjustX;
		Mag.Y  *= Mag.AdjustY;
		Mag.Z  *= Mag.AdjustZ;
		Temp.T -= Temp.OffsetT;

		Delay_10ms(10);
		test();
		printf("Acc: %f,%f,%f\n", Acc.TrueX, Acc.TrueY, Acc.TrueZ);
//		printf("Gyr: %d,%d,%d\n", Gyr.X, Gyr.Y, Gyr.Z);

	
	}

}
