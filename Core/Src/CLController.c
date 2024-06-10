/*
 * CLController.c
 *
 *  Created on: May 29, 2024
 *      Author: Jared Sinasohn
 */
#include "CLController.h"

uint32_t run(CLController* con,  int32_t measured){
	con->curr = measured;
	con->curr_time = HAL_GetTick();
	con->err = con->setpoint - con->curr;
	con->err_acc = con->err_acc + con->curr;
	con->eff = con->kp * con->err + con->ki * con->err_acc;
	if(con->kd > 0){
		con->prev_err_list[con->prev_err_index] = con->err;
		con->prev_err_index += 1;
	}
	if(con->kd > 0 && con->prev_err_index >= con->prev_err_list_length){
		con->slope = (con->prev_err_list[con->prev_err_index-1]-con->prev_err_list[0])/(con->curr_time - con->initial_time);
		con->eff += con->kd * con->slope;
		con->prev_err_index = 0;
	}
	con->initial_time = con->curr_time;
	return con->eff;
}
void reset_controller(CLController* con){
	con->err = 0;
	con->eff = 0;
	con->err_acc = 0;
	con->slope = 0;
	con->curr = 0;
	con->initial_time = HAL_GetTick();
	con->curr_time = HAL_GetTick();
}
