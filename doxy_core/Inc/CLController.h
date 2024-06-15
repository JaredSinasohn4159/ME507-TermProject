/*!
 * @file CLController.h
 *
 * @date May 29, 2024
 * @author Jared Sinasohn
 * @brief This file defines the abstract class and methods for a closed-loop PIDF controller.
 */


#ifndef SRC_CLCONTROLLER_H_
#define SRC_CLCONTROLLER_H_

#include "stm32L4xx_hal.h"

/*!
 * @class CLController CLController.h
 *
 * @brief An implementation of a PIDF (F = Feedforward constant) closed loop controller.  It is a generic controller that takes in a measurement, controller constants, and a setpoint and outputs an effort to be sent to the actuator.
 *
 * @param kp The propotional control constant multiplied by 10^6 for accuracy reasons
 * @param ki The integral control constant multiplied by 10^6 for accuracy reasons
 * @param kd The derivative control constant multiplied by 10^6 for accuracy reasons
 * @param kf The Feedforward constant multiplied by 10^6 for accuracy reasons.  The feedforward constant allows for simpler speed control in this project.
 * @param setpoint The reference input value of the controller
 * @param eff The output effort calculated by the closed loop controller
 * @param curr The currently measured value of the sensor in the loop
 * @param err The error between the setpoint and measured values, used for proportional control
 * @param err_acc How much error has been accumulating over time, used to estimate an integral control
 * @note the following parameters all have to do with derivative control.  Due to the nasty things noise can do to derivative control, a simple impulse filter has been implemented.  It works by accumulating a list of error values and taking the slope vs time of that list once the list is full, and using that derivative list until the list fills again.  This effectively filters out impulses due to the averaging, and thus creates a smoother, noise-free derivative control.
 * @param prev_err_index the index for the list of previous errors
 * @param initial_time The initial time when the controller began calculating derivative error data
 * @param curr_time The current time at which derivitive error has been calculated
 * @param slope The overall slope of the list of derivative errors
 * @param prev_err_list_length The number of error samples to be taken for the list, as chosen by the control programmer
 * @param prev_err_list[] The list previous error values are stored in.  @Attention this must be the same length as the previous_err_list_length
 */

typedef struct{
	uint32_t kp;
	uint32_t ki;
	uint32_t kd;
	uint32_t kf;
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


/*! This function runs the Closed Loop Controller based on the parameters set up initially by the programmer
 *
 * @param con The CLController instance to run the plant with
 * @param measured The value measured by the sensor to be fed back into the controller
 *
 * @returns eff The effort/output calculated by the Controller to send to the actuator
 *
 */

uint32_t run(CLController* con,  int32_t measured);
/*! This function hard resets the controller (keeping controller gains the same)
 *
 * @param con The CLController instance to run the plant with
 *
 */
void reset_controller(CLController* con);



#endif /* SRC_CLCONTROLLER_H_ */
