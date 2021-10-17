
/**
 ******************************************************************************
  * File Name          : flash_wb55.c
  * Description        : Implementation of read/write to flash.
  *
 *******************************************************************************/

#include <flash_wb55.h>
#include "string.h"
#include "stdio.h"


#define USER_DATA_SIZE 1
/* Start user data from the beginning of the next page. */
#define USER_DATA_START  (((uint32_t)_USER_DATA_ + FLASH_PAGE_SIZE) & 0x0FFFF000)
/* End of 512K */
#define USER_DATA_END (FLASH_BASE + 0x80000)

/* Label end of unused flash area by declaring _USER_DATA_ at the last section
 * named .user_data in FLASH. Make sure to declare the following code in .ld file.
 *
 * .user_array :
 * {
 *   KEEP(*(SORT(.*user_data*) ))
 * } >FLASH
 *
 */
__attribute__ ((section (".user_data"))) unsigned char _USER_DATA_[USER_DATA_SIZE] = {0};

static uint8_t tmp[4];


static uint32_t GetPage(uint32_t Address)
{
  for (int indx = 0; indx < FLASH_PAGE_NB; indx++)
  {
	  if((Address < (FLASH_BASE + (FLASH_PAGE_SIZE *(indx + 1))) ) && (Address >= (FLASH_BASE + FLASH_PAGE_SIZE * indx)))
	  {
		  return (FLASH_BASE + FLASH_PAGE_SIZE*indx);
	  }
  }

  return 0;
}

static uint32_t GetPageIndex(uint32_t Address)
{
  for (int indx = 0; indx < FLASH_PAGE_NB; indx++)
  {
	  if((Address < (FLASH_BASE + (FLASH_PAGE_SIZE *(indx + 1))) ) && (Address >= (FLASH_BASE + FLASH_PAGE_SIZE * indx)))
	  {
		  return indx;
	  }
  }

  return 0;
}

void float2Bytes(uint8_t * ftoa_bytes_temp, float float_variable)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    thing.a = float_variable;

    for (uint8_t i = 0; i < 4; i++) {
      ftoa_bytes_temp[i] = thing.bytes[i];
    }

}

float Bytes2float(uint8_t * ftoa_bytes_temp)
{
    union {
      float a;
      uint8_t bytes[4];
    } thing;

    for (uint8_t i = 0; i < 4; i++) {
        thing.bytes[i] = ftoa_bytes_temp[i];
    }

    float float_variable =  thing.a;
    return float_variable;
}

uint32_t flash_write (uint32_t offset, uint32_t *Data, uint16_t numberofwords)
{
    HAL_StatusTypeDef status;
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;
    int sofar = 0;
    uint32_t StartPageAddress = USER_DATA_START + offset;

    if ((StartPageAddress + numberofwords * 4) > USER_DATA_END) {
        return HAL_FLASH_ERROR_SIZ;
    }

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR |FLASH_FLAG_PGSERR | FLASH_FLAG_OPTVERR);

    /* Erase the user Flash area */
    uint32_t StartPage = GetPage(StartPageAddress);
    uint32_t StartPageIndex = GetPageIndex(StartPageAddress);

    uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
    uint32_t EndPage = GetPage(EndPageAdress);

    /* Fill EraseInit structure */
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = StartPageIndex;
    EraseInitStruct.NbPages     = ((EndPage - StartPage)/FLASH_PAGE_SIZE) +1;

    /* Verify that next operation can be proceed */
	int attempts = 5;
	do {
	   status = FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

	} while ((status != HAL_OK) && attempts-- >= 0);

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		/* Error occurred while page erase.*/
        return HAL_FLASH_GetError();
    }

    /* Program the user Flash area word by word*/
    while (sofar < numberofwords)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, StartPageAddress, Data[sofar]) == HAL_OK)
        {
            StartPageAddress += 8;  /* StartPageAddress += 2 for half word, 8 for double word */
            sofar++;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return HAL_FLASH_GetError ();
        }
    }

    HAL_FLASH_Lock();

    return 0;
}

void flash_read (uint32_t offset, uint32_t *RxBuf, uint16_t numberofwords)
{
	uint32_t StartPageAddress = USER_DATA_START + offset;

    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR | FLASH_FLAG_OPTVERR);

    while (1)
    {
        *RxBuf = *(__IO uint32_t *)StartPageAddress;
        StartPageAddress += 8; /* 8 for doubleword */
        RxBuf++;
        if (!(numberofwords--)) break;
    }

    HAL_FLASH_Lock();
}
void convert_to_str (uint32_t *data, char *buf)
{
    int numberofbytes = ((strlen((char *)data) / 4) + ((strlen((char *)data) % 4) != 0)) *4;

    for (int i = 0; i < numberofbytes; i++)
    {
        buf[i] = data[i / 4] >>  (8 * (i % 4));
    }
}


void flash_write_num (uint32_t offset, float Num)
{
    float2Bytes(tmp, Num);
    flash_write (offset, (uint32_t *)tmp, 1);
}


float flash_read_num (uint32_t offset)
{
    uint8_t buffer[4];
    float value;

    flash_read(offset, (uint32_t *)buffer, 1);
    value = Bytes2float(buffer);
    return value;
}

