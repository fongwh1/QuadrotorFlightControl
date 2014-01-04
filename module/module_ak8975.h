/* #include "module_ak8975.h" */
// "terrestrial magnetism" in the X-axis, Y-axis, and Z-axis
#ifndef __MODULE_AK8975_H
#define __MODULE_AK8975_H

#include "stm32f4xx.h"
/*=====================================================================================================*/
/*=====================================================================================================*/
#define AK8975_I2C_ADDR     ((u8)0x18)
#define AK8975_Device_ID    ((u8)0x48)

#define	AK8975_WIA          ((u8)0x00)
#define	AK8975_INFO         ((u8)0x01)
#define	AK8975_ST1          ((u8)0x02)
#define	AK8975_HXL          ((u8)0x03)
#define	AK8975_HXH          ((u8)0x04)
#define	AK8975_HYL          ((u8)0x05)
#define	AK8975_HYH          ((u8)0x06)
#define	AK8975_HZL          ((u8)0x07)
#define	AK8975_HZH          ((u8)0x08)
#define	AK8975_ST2          ((u8)0x09)
#define	AK8975_CNTL         ((u8)0x0A)
#define	AK8975_RSV          ((u8)0x0B)
#define	AK8975_ASTC         ((u8)0x0C)
#define	AK8975_TS1          ((u8)0x0D)
#define	AK8975_TS2          ((u8)0x0E)
#define	AK8975_I2CDIS       ((u8)0x0F)
#define	AK8975_ASAX         ((u8)0x10)
#define	AK8975_ASAY         ((u8)0x11)
#define	AK8975_ASAZ         ((u8)0x12)

#define AK8975_1200uT       ((float)0.3f)  // 0.3 uT/LSB
/*=====================================================================================================*/
/*=====================================================================================================*/
void AK8975_Init( void );
/*=====================================================================================================*/
/*=====================================================================================================*/
#endif
