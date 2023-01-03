#ifndef __INTERNAL_FLASH_H
#define	__INTERNAL_FLASH_H

#include "stm32f10x.h"

/* STM32大容量产品每页大小2KByte，中、小容量产品每页大小1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)	//2048
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

//写入的起始地址与结束地址
#define WRITE_START_ADDR  ((uint32_t)0x08008000)
#define WRITE_END_ADDR    ((uint32_t)0x0800C000)

struct WriteInfo
{
	unsigned short write_province_id;

	unsigned short write_city_id;

	unsigned char write_manufacturer_id[5];

	unsigned char write_terminal_model[20];

	unsigned char write_terminal_id[7];

	unsigned char write_car_plate_color;

	unsigned char write_car_plate_num[12];
};

struct ReadInfo
{
	unsigned short read_province_id;

	unsigned short read_city_id;

	unsigned char read_manufacturer_id[5];

	unsigned char read_terminal_model[20];

	unsigned char read_terminal_id[7];

	unsigned char read_car_plate_color;

	unsigned char read_car_plate_num[12];
};







typedef enum 
{
	FAILED = 0, 
  PASSED = !FAILED
} TestStatus;

void FLASH_WriteByte(uint32_t addr , uint8_t *p , uint16_t Byte_Num);
void Internal_ReadFlash(uint32_t addr , uint8_t *p , uint16_t Byte_Num);

#endif /* __INTERNAL_FLASH_H */

