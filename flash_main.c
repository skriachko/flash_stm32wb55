/**
 ******************************************************************************
  * File Name          : flash_main.c
  * Description        : Test read/write to flash.
  *
  * Created on: Oct, 2021
  * Author: Sergii Kriachko
  *
 *******************************************************************************/

#include <flash_wb55.h>
#include <string.h>

#define RUNNING_ON_DEV_KIT

char *str_write = "OneTwoThreeFourFive\0";
char str_read[100];

uint32_t data2save[] = {0x01, 0x02, 0x03, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9};
uint32_t data2read[30];

int number_write = 123;
float float_write = 123.456;
float float_read;

#ifdef RUNNING_ON_DEV_KIT
#define LD2_Pin GPIO_PIN_0
#define LD2_GPIO_Port GPIOB
#endif

static void turn_on_led(void)
{
#ifdef RUNNING_ON_DEV_KIT
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Configure GPIO pins : LD2_Pin LD3_Pin LD1_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET);
	HAL_Delay(1000);
#endif
}

/* Function to test write and read to/from flash */
void eeprom_test(void)
{
    flash_write(0x1000, (uint32_t *)data2save, 9);
    flash_read(0x1000 , data2read, 9);

    int numofwords = (strlen(str_write)/4)+((strlen(str_write)%4)!=0);
    // Check if we already have data there
    flash_read(0, data2read, numofwords);
    convert_to_str(data2read, str_read);

    flash_write(0, (uint32_t *)str_write, numofwords);
    flash_read(0, data2read, numofwords);
    convert_to_str(data2read, str_read);

    float_read = flash_read_num(0x2000);
    if (number_write == float_read) {
        /* Blink if we read what we saved to flash */
        turn_on_led();
    } else {
        flash_write_num(0x2000, number_write);
        float_read = flash_read_num(0x2000);
    }

    float_read = flash_read_num(0x3000);
    flash_write_num(0x3000, float_write);
    float_read = flash_read_num(0x3000);

}




