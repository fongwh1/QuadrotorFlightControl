#include "stm32f4_system.h"
#include "stm32f4_i2c.h"
#include "stm32f4_delay.h"

#include "module_board.h" // for the LED
#include "module_sensor.h"
#include "module_ak8975.h"
#include "module_mpu6050.h"
#include "module_mpu9150.h"
#include "module_motor.h"

#include "algorithm_moveAve.h" // for function MoveAve_SMA/MoveAve_WMA
#include "algorithm_pid.h"
#include "algorithm_mathUnit.h"
#include "algorithm_quaternion.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "sensor.h"
#include <math.h>

#define	SAMPLING_TIMES	100
#define DELTA_T	(float)0.11
#define GYROSCOPE_SENSITIVITY 65.536

extern SensorAcc Acc;
extern SensorGyr Gyr;
extern SensorMag Mag;
extern SensorTemp Temp;

u8 IMU_Buf[20] = {0};

float angle, angle_vec = 0;

void test(void)
{
	int a = 1, b = 0;
	float c = 0;

	c = atan2(a,b);

	printf("%f,  %f,  %f,   %f,   %f\n",-0.1,0.01,0.001,-0.0001,0.00001);

}

void sensor_offset_sampling(void)
{
	int sampling_times = SAMPLING_TIMES;

	Delay_10ms(10);

	int OffsetX = 0,OffsetY = 0, OffsetZ = 0,gOffsetX = 0,gOffsetY = 0,gOffsetZ = 0;

	Acc.OffsetX = 0;
	Acc.OffsetY = 0;
	Acc.OffsetX = 0;

	Gyr.OffsetX = 0;
	Gyr.OffsetY = 0;
	Gyr.OffsetZ = 0;

	while(sampling_times > 0)
	{
		MPU9150_Read(IMU_Buf);
		LED_B = ~LED_B;

                OffsetX  += (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
                OffsetY  += (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
                OffsetZ  += (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
                Temp.T = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
                gOffsetX  += (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
                gOffsetY  += (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
                gOffsetZ  += (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);
                Mag.X  = (s16)((IMU_Buf[15] << 8) | IMU_Buf[14]);
                Mag.Y  = (s16)((IMU_Buf[17] << 8) | IMU_Buf[16]);
                Mag.Z  = (s16)((IMU_Buf[19] << 8) | IMU_Buf[18]);
	
		sampling_times--;

		Delay_10ms(1);

//		printf("%f %d %d %d\n", dt, OffsetX, OffsetY, OffsetZ);

	}

	Acc.OffsetX = (s16)(OffsetX/SAMPLING_TIMES);
	Acc.OffsetY = (s16)(OffsetY/SAMPLING_TIMES);
	Acc.OffsetZ = (s16)(OffsetZ/SAMPLING_TIMES);

	Gyr.OffsetX = (s16)(gOffsetX/SAMPLING_TIMES);
	Gyr.OffsetY = (s16)(gOffsetY/SAMPLING_TIMES);
	Gyr.OffsetZ = (s16)(gOffsetZ/SAMPLING_TIMES);

//	printf("offset sampling result:X: %d Y:%d Z:%d \n",Acc.OffsetX,Acc.OffsetY,Acc.OffsetZ);
//	printf("offset sampling result:X: %d Y:%d Z:%d \n",Gyr.OffsetX,Gyr.OffsetY,Gyr.OffsetZ);

	Acc.X  = (s16)((IMU_Buf[0]  << 8) | IMU_Buf[1]);
	Acc.Y  = (s16)((IMU_Buf[2]  << 8) | IMU_Buf[3]);
	Acc.Z  = (s16)((IMU_Buf[4]  << 8) | IMU_Buf[5]);
	Temp.T = (s16)((IMU_Buf[6]  << 8) | IMU_Buf[7]);
	Gyr.X  = (s16)((IMU_Buf[8]  << 8) | IMU_Buf[9]);
	Gyr.Y  = (s16)((IMU_Buf[10] << 8) | IMU_Buf[11]);
	Gyr.Z  = (s16)((IMU_Buf[12] << 8) | IMU_Buf[13]);

	Acc.X -= Acc.OffsetX;
  	Acc.Y -= Acc.OffsetY;
  	Acc.Z -= Acc.OffsetZ;
  	Gyr.X -= Gyr.OffsetX;
  	Gyr.Y -= Gyr.OffsetY;
  	Gyr.Z -= Gyr.OffsetZ;


	/*Mode Quaternion*/
	Acc.TrueX = Acc.X*MPU9150A_4g;        // g/LSB
	Acc.TrueY = Acc.Y*MPU9150A_4g;        // g/LSB
    Acc.TrueZ = Acc.Z*MPU9150A_4g;        // g/LSB
    Gyr.TrueX = Gyr.X*MPU9150G_2000dps;   // dps/LSB
    Gyr.TrueY = Gyr.Y*MPU9150G_2000dps;   // dps/LSB
    Gyr.TrueZ = Gyr.Z*MPU9150G_2000dps;   // dps/LSB
    Temp.TrueT = Temp.T*MPU9150T_85degC;  // degC/LSB
/*
    Ellipse[3] = ( Mag.X*arm_cos_f32(Mag.EllipseSita)+Mag.Y*arm_sin_f32(Mag.EllipseSita))/Mag.EllipseB;
	Ellipse[4] = (-Mag.X*arm_sin_f32(Mag.EllipseSita)+Mag.Y*arm_cos_f32(Mag.EllipseSita))/Mag.EllipseA;
*/
	AngE.Pitch = toDeg(atan2f(Acc.TrueY, Acc.TrueZ));
	AngE.Roll  = toDeg(-asinf(Acc.TrueX));
	AngE.Yaw   = /*toDeg(atan2f(Ellipse[3], Ellipse[4]))+*/180.0f;
		
	Quaternion_ToNumQ(&NumQ, &AngE);
	

	LED_B = 0;
	Delay_10ms(100);
	LED_B = 1;
}

void complementary_filter()
{
	angle = (0.98*(angle + Gyr.X * DELTA_T) + (0.02*(Acc.TrueX)));
	printf("%f\n",angle);
}

void sensor_task(void * pvParameters)
{
	u8 print_count = 0;
	sensor_offset_sampling();

	float running_time = 0;

	while(1)
	{
		u16 Final_M1 = 0;
		u16 Final_M2 = 0;
		u16 Final_M3 = 0;
		u16 Final_M4 = 0;
		
		s16 Thr = 0, Pitch = 0, Roll = 0, Yaw = 0;

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


		Acc.X -= Acc.OffsetX;
		Acc.Y -= Acc.OffsetY;
		Acc.Z -= Acc.OffsetZ;
		Gyr.X  -= Gyr.OffsetX;
		Gyr.Y  -= Gyr.OffsetY;
		Gyr.Z  -= Gyr.OffsetZ;
		Mag.X  *= Mag.AdjustX;
		Mag.Y  *= Mag.AdjustY;
		Mag.Z  *= Mag.AdjustZ;
		Temp.T -= Temp.OffsetT;

		//complementary_filter();

		Delay_1ms(8);

//		if( print_count == 0){
//		printf("%f %f %f %f\n",running_time, Acc.TrueX, Acc.TrueY, Acc.TrueZ);
		running_time += DELTA_T;// should be dt
//		printf("Gyr: %d %d %d\n", Gyr.X, Gyr.Y, Gyr.Z);
//		print_count = 0;
//		}
//		print_count++;
	
		/* 加權移動平均法 Weighted Moving Average */
/*		Acc.X = (s16)MoveAve_WMA(Acc.X, ACC_FIFO[0], 8);
		Acc.Y = (s16)MoveAve_WMA(Acc.Y, ACC_FIFO[1], 8);
		Acc.Z = (s16)MoveAve_WMA(Acc.Z, ACC_FIFO[2], 8);
		Gyr.X = (s16)MoveAve_WMA(Gyr.X, GYR_FIFO[0], 8);
		Gyr.Y = (s16)MoveAve_WMA(Gyr.Y, GYR_FIFO[1], 8);
		Gyr.Z = (s16)MoveAve_WMA(Gyr.Z, GYR_FIFO[2], 8);
		Mag.X = (s16)MoveAve_WMA(Mag.X, MAG_FIFO[0], 64);
		Mag.Y = (s16)MoveAve_WMA(Mag.Y, MAG_FIFO[1], 64);
		Mag.Z = (s16)MoveAve_WMA(Mag.Z, MAG_FIFO[2], 64);
*/		
		/* To Physical */
		Acc.TrueX = Acc.X*MPU9150A_4g;        // g/LSB
		Acc.TrueY = Acc.Y*MPU9150A_4g;        // g/LSB
		Acc.TrueZ = Acc.Z*MPU9150A_4g;        // g/LSB
		Gyr.TrueX = Gyr.X*MPU9150G_2000dps;   // dps/LSB
		Gyr.TrueY = Gyr.Y*MPU9150G_2000dps;   // dps/LSB
		Gyr.TrueZ = Gyr.Z*MPU9150G_2000dps;   // dps/LSB
		Mag.TrueX = Mag.X*MPU9150M_1200uT;    // uT/LSB
		Mag.TrueY = Mag.Y*MPU9150M_1200uT;    // uT/LSB
		Mag.TrueZ = Mag.Z*MPU9150M_1200uT;    // uT/LSB
		Temp.TrueT = Temp.T*MPU9150T_85degC;  // degC/LSB
		
		/* Get Attitude Angle */
		AHRS_Update();

      /* Get ZeroErr */
		PID_Pitch.ZeroErr = 0;//(float)((s16)Exp_Pitch/4.5f);
		PID_Roll.ZeroErr  = 0;//(float)((s16)Exp_Roll/4.5f);
		PID_Yaw.ZeroErr   = 0;//(float)((s16)Exp_Yaw)+180.0f;
		
		/* PID */
		Roll  = (s16)PID_AHRS_Cal(&PID_Roll,   AngE.Roll,  Gyr.TrueX);
		Pitch = (s16)PID_AHRS_Cal(&PID_Pitch,  AngE.Pitch, Gyr.TrueY);
//      Yaw   = (s16)PID_AHRS_CalYaw(&PID_Yaw, AngE.Yaw,   Gyr.TrueZ);
		Yaw   = (s16)(PID_Yaw.Kd*Gyr.TrueZ);
		Thr   = 0;//(s16)Exp_Thr;
		
		/* Motor Ctrl */
		Final_M1 = PWM_Motor1 + Thr + Pitch + Roll + Yaw;
		Final_M2 = PWM_Motor2 + Thr - Pitch + Roll - Yaw;
		Final_M3 = PWM_Motor3 + Thr - Pitch - Roll + Yaw;
		Final_M4 = PWM_Motor4 + Thr + Pitch - Roll - Yaw;
		
		//printf("EXP_Motor:%d %d %d %d\n", Final_M1, Final_M2, Final_M3, Final_M4);
		
		//Motor_Control(Final_M1, Final_M2, Final_M3, Final_M4);
	}

}


