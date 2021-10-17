/**
 ******************************************************************************
  * File Name          : flash_wb55.h
  * Description        : read/write to flash interface.
  *
  * Created on: Oct, 2021
  * Author: Sergii Kriachko
  *
 *******************************************************************************/

#ifndef INC_FLASH_WB55_H_
#define INC_FLASH_WB55_H_

#include "stm32wbxx_hal.h"


uint32_t flash_write (uint32_t offset, uint32_t *Data, uint16_t numberofwords);
void flash_read (uint32_t offset, uint32_t *RxBuf, uint16_t numberofwords);
void convert_to_str (uint32_t *data, char *buf);
void flash_write_num (uint32_t offset, float Num);
float flash_read_num (uint32_t offset);




#endif /* INC_FLASH_WB55_H_ */
