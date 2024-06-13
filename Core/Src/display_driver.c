/*
 * display_driver.c
 *
 *  Created on: Jun 9, 2024
 *      Author: Jared Sinasohn
 */
#include "display_driver.h"
uint16_t note_addresses[12] = {0b1101100001111000,0b1101101101110101,0b1101101101110100,0b1101101100000000,0b1110001111001001,0b1110001111001000,0b1101101100110001,0b1101101100110000,0b1101000000110000,0b1101101101010001,0b1101101101010000,0b1101100001111001};
//uint16_t* note_addresses = [1101100001111000,1101101101110101,1101101101110100,1101101100000000,1110001111001001,1110001111001000,1101101100110001,1101101100110000,1101000000110000,1101101101010001,1101101101010000,1101100001111001];
//uint16_t* note_addresses = {55416,56181,56180,56064,58313,58312,56113,56112,53296,56145,56144,55417};
uint16_t disp_addr = 0b11000001;
uint8_t Pitch_Message[] = "Current Pitch: ";
uint8_t Pitch_Buffer[50] = {0};
uint8_t display_note(Display* disp, uint8_t note){
	if(disp->curr_note == note){
		return disp->curr_note;
	}
	disp->curr_note = note;
	/*HAL_I2C_Mem_Write(disp->hi2c, disp_addr, 0x14, 1, (uint8_t*)0b01010101, 1, 250);
	HAL_I2C_Mem_Write(disp->hi2c, disp_addr, 0x15, 1, (uint8_t*)0b01010101, 1, 250);
	HAL_I2C_Mem_Write(disp->hi2c, disp_addr, 0x16, 1, (uint8_t*)0b01010101, 1, 250);
	HAL_I2C_Mem_Write(disp->hi2c, disp_addr, 0x17, 1, (uint8_t*)0b01010101, 1, 250);
	disp->curr_note = note;
	HAL_I2C_Master_Transmit((disp->hi2c), disp_addr, ((uint8_t*)&note_addresses[disp->curr_note]),2,100);*/
	switch(disp->curr_note){
			case 0:
				sprintf(Pitch_Buffer, "A  \r\n");
				break;
			case 1:
						sprintf(Pitch_Buffer, "Bb \r\n");
						break;
			case 2:
						sprintf(Pitch_Buffer, "B  \r\n");
						break;
			case 3:
						sprintf(Pitch_Buffer, "C  \r\n");
						break;
			case 4:
						sprintf(Pitch_Buffer, "Db \r\n");
						break;
			case 5:
						sprintf(Pitch_Buffer, "D  \r\n");
						break;
			case 6:
						sprintf(Pitch_Buffer, "Eb \r\n");
						break;
			case 7:
						sprintf(Pitch_Buffer, "E  \r\n");
						break;
			case 8:
						sprintf(Pitch_Buffer, "F  \r\n");
						break;
			case 9:
						sprintf(Pitch_Buffer, "Gb \r\n");
						break;
			case 10:
						sprintf(Pitch_Buffer, "G  \r\n");
						break;
			case 11:
						sprintf(Pitch_Buffer, "Ab \r\n");
						break;
		}
		HAL_UART_Transmit(disp->huart, Pitch_Message, sizeof(Pitch_Message), 10000);
		HAL_UART_Transmit(disp->huart, Pitch_Buffer, sizeof(Pitch_Buffer), 10000);
	return disp->curr_note;
}

