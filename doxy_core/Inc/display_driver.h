/*!
 * @file display_driver.h
 *
 * @date May 30, 2024
 * @author Jared Sinasohn
 * @brief The header file for the i2c driven 16 segment display class
 */

#ifndef SRC_DISPLAY_DRIVER_H_
#define SRC_DISPLAY_DRIVER_H_

#include "stm32l4xx_hal.h"
#include "stdio.h"

/*!
 * @class Display display_driver.h
 *
 * @brief The class that implements the control of the TLC59116 16-Channel FM+ I2C-Bus Constant-Current LED Sink Driver to display the current note on the tuner.
 *
 * @param hi2c The i2c bus the display driver is on
 * @param curr_note The current note to display, a value from 0-11 mapping from A-Ab
 * @param huart The uart bus to provide serial communication to a computer console for displaying the current note in debug note.  In the final version, this parameter will be removed as it is unnecessary to run the actual display, but for now we need it to see what notes are being displayed to the console.
 */
typedef struct{
	I2C_HandleTypeDef* hi2c;
	uint8_t curr_note;
	USART_TypeDef * huart;
}Display;

/*! This function displays the current note on the display.
 *  @param disp The display instance
 *  @param note, the note to be displayed from 0-11.
 *  @return curr_note The note that is displayed.
 */
uint8_t display_note(Display* disp, uint8_t note);
#endif /* SRC_DISPLAY_DRIVER_H_ */
