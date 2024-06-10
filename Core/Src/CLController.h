/*
 * CLController.h
 *
 *  Created on: May 29, 2024
 *      Author: Jared Sinasohn
 */

#ifndef SRC_CLCONTROLLER_H_
#define SRC_CLCONTROLLER_H_

#include "stm32L4xx_hal.h"

typedef struct{
	float kp;
	float ki;
	float kd;
	int32_t setpoint;
	int32_t eff;
	int32_t curr;
	int32_t err;
	int32_t err_acc;
	uint8_t prev_err_index;
	uint32_t initial_time;
	uint32_t curr_time;
	int32_t slope;
	uint32_t prev_err_list_length;
	int32_t prev_err_list[];
}CLController;

uint32_t run(CLController* con,  int32_t measured);
void reset_controller(CLController* con);



#endif /* SRC_CLCONTROLLER_H_ */
