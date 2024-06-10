/*
 * display_driver.h
 *
 *  Created on: May 30, 2024
 *      Author: Jared Sinasohn
 */

#ifndef SRC_DISPLAY_DRIVER_H_
#define SRC_DISPLAY_DRIVER_H_

#include "stm32l4xx_hal.h"

typedef struct{
	I2C_HandleTypeDef* hi2c;
	uint8_t curr_note;
}Display;

uint8_t display_note(Display* disp, uint8_t note);
#endif /* SRC_DISPLAY_DRIVER_H_ */
