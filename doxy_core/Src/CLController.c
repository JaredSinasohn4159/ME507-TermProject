/*!
 * @file CLController.c
 *
 * @date May 29, 2024
 * @author Jared Sinasohn
 * @brief this file implements the methods for the CLController class.
 */
#include "CLController.h"

/*! This function runs the Closed Loop Controller based on the parameters set up initially by the programmer
 *
 * @param con The CLController instance to run the plant with
 * @param measured The value measured by the sensor to be fed back into the controller
 *
 * @return eff The effort/output calculated by the Controller to send to the actuator
 * @attention This function should be run at a rate slower than 1ms so as to not have zero time differential (if you are not using derivative control you can run at whatever speed you desire.)
 */
uint32_t run(CLController* con,  int32_t measured){
	//! store the sensor value as the current value
	con->curr = measured;
	//! get the current time of simulation.
	con->curr_time = HAL_GetTick();
	//! calculate the error between the setpoint and the measured value
	con->err = con->setpoint - con->curr;
	//! add this error to the accumulated error, but only if integral control has been implemented.
	if(con->ki > 0){
		con->err_acc = con->err_acc + con->err;
	}
	//! set the effort to be the proportional control plus integral control plus feed-forward control
	con->eff = (con->kf * con->setpoint)/1000000 + (con->kp * con->err)/1000000 + (con->ki * con->err_acc)/1000000;
	//! if derivative control is enabled, add the current error to the list of errors and increment the index
	if(con->kd > 0){
		con->prev_err_list[con->prev_err_index] = con->err;
		con->prev_err_index += 1;
	}
	//! if the list of errors is full, calculate the slope of the list and multiply it by the derivative constant and add it to the effort
	if(con->kd > 0 && con->prev_err_index >= con->prev_err_list_length){
		con->slope = ((con->prev_err_list[con->prev_err_index-1]-con->prev_err_list[0]))*1000/(con->curr_time - con->initial_time);
		con->eff += con->kd * con->slope;
		con->prev_err_index = 0;
		con->initial_time = con->curr_time;
	}
	return con->eff;
}
/*! This function hard resets the controller (keeping controller gains the same)
 *
 * @param con The CLController instance to run the plant with
 *
 */
void reset_controller(CLController* con){
	con->err = 0;
	con->eff = 0;
	con->err_acc = 0;
	con->slope = 0;
	con->curr = 0;
	con->initial_time = HAL_GetTick();
	con->curr_time = HAL_GetTick();
}
