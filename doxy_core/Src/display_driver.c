/*!
 * @file display_driver.c
 *
 * @date Jun 9, 2024
 * @author Jared Sinasohn
 * @brief Implements the methods from the Display class.
 */
#include "display_driver.h"
/*!
 * @var note_addresses
 * The values used to light up the 16 segment display, each position in the list corresponds to a note, each bit in the 16 bit numbers corresponds to a segment in the display
 */
uint16_t note_addresses[12] = {0b1101100001111000,0b1101101101110101,0b1101101101110100,0b1101101100000000,0b1110001111001001,0b1110001111001000,0b1101101100110001,0b1101101100110000,0b1101000000110000,0b1101101101010001,0b1101101101010000,0b1101100001111001};
/*!
 * @var disp_addr
 * The address of the display driver on the i2c bus
 */
uint16_t disp_addr = 0b11000001;
uint8_t Pitch_Message[] = "Current Pitch: ";
uint8_t Pitch_Buffer[50] = {0};

/*! This function displays the current note on the display.
 *  @param disp The display instance
 *  @param note The note to be displayed from 0-11.
 *  @return curr_note The note that is displayed.
 *  @attention The Current function uses uart to display the current note, but at the end would use i2c to the driver.
 */
uint8_t display_note(Display* disp, uint8_t note){
	if(disp->curr_note == note){
		return disp->curr_note;
	}
	disp->curr_note = note;
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

