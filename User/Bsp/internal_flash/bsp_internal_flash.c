#include "./internal_flash/bsp_internal_flash.h"   
#include "./usart/usart.h"
#include "string.h"

//int InternalFlash_Test(uint8_t Data)
/*void Internal_WriteFlash(uint32_t Address, uint8_t *Data, int dataLen)
{
	uint32_t EraseCounter = 0x00; 	//��¼Ҫ��������ҳ
	//uint32_t Address = 0x00;				//��¼д��ĵ�ַ
	//uint32_t Data = 0x3210ABCD;			//��¼д�������
	uint32_t NbrOfPage = 0x00;			//��¼д�����ҳ
	int writeCounter = 0;
	
	FLASH_Status FLASHStatus = FLASH_COMPLETE; //��¼ÿ�β����Ľ��	
	TestStatus MemoryProgramStatus = PASSED;//��¼�������Խ��
	

  // ����
  FLASH_Unlock();

  // ����Ҫ��������
  NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

  // ������б�־λ 
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  // ��ҳ��
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
	}
  
  // ���ڲ�FLASHд������ 
  //Address = WRITE_START_ADDR;
	
	for(writeCounter = 0; (writeCounter < dataLen)&&(FLASHStatus == FLASH_COMPLETE); writeCounter++)
	{
		FLASHStatus = FLASH_ProgramWord(Address, Data[writeCounter]);            
		Address = Address + 1;                                                                 
	}
	
  FLASH_Lock();
}

void Internal_ReadFlash(uint8_t *pData, uint32_t dataLen)
{
    uint32_t i = 0;
    
    uint32_t address = WRITE_START_ADDR;
    
    for(i = 0; i < dataLen; i++)
    {
        pData[i] = (*(__IO uint8_t*) address);                                                
        address += 1;                                                                                
    }
}
*/
void FLASH_WriteWord(uint32_t Address , uint32_t Data)
{	
  /* ���� */
  FLASH_Unlock();

  /* ������б�־λ */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  /* ��ҳ����*/
	FLASH_ErasePage(Address);

  FLASH_ProgramWord(Address, Data);

  FLASH_Lock();
}



void FLASH_WriteByte(uint32_t addr , uint8_t *p , uint16_t Byte_Num)
{
		uint32_t HalfWord;
		Byte_Num = Byte_Num/2;
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY | FLASH_FLAG_OPTERR);
		FLASH_ErasePage(addr);
		while(Byte_Num --)
		{
						HalfWord=*(p++);
						HalfWord|=*(p++)<<8;
						FLASH_ProgramHalfWord(addr, HalfWord);
						addr += 2;
		}
		FLASH_Lock();
}


void Internal_ReadFlash(uint32_t addr , uint8_t *p , uint16_t Byte_Num)
{
	while(Byte_Num--)
	{
	 *(p++)=*((uint8_t*)addr++);
	}
}



/*
uint8_t FLASH_ReadByte(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}

void FLASH_Read(u8 *pBuffer)
{
	int i;
	uint32_t address = WRITE_START_ADDR;
	//int NumToRead = WRITE_END_ADDR;
	

	for(i=0; i<50;i++)
	{
		pBuffer[i]=FLASH_ReadByte(address);
		address=address+1;
	}
}
*/

//void FlashRead(uint32_t addr, uint8_t *buffer)
//{
//  memcpy(buffer, (void *)addr, 50);
//}
//uint16_t FLASH_ReadHalfWord(uint32_t address)
//{
//   return *(__IO uint16_t*)address; 
//}


  // ���д��������Ƿ���ȷ 
 // Address = WRITE_START_ADDR;
	//printf("%x \r\n", Data);
	//printf("%x \r\n",(*(__IO uint32_t*) Address));
 // while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
  //{
    //if((*(__IO uint8_t*) Address) != Data[])
//{
//MemoryProgramStatus = FAILED;
//}
//Address += 1;
  //}
	//return MemoryProgramStatus;
//}




