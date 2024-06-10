/*
 *  @file motor_driver.c
 *
 *  @date Apr 25, 2024
 *  @author Jared Sinasohn
 *
 *  @brief this file implements the Motor struct to allow for pseudo
 *  object oriented programming motor control.
 */

#include "motor_driver.h"

void motor_set_duty_cycle(Motor* motor, int32_t doot){
	/* @brief This function implements the duty cycle setting of the motor.  It takes in
	 * the motor struct and a duty cycle from -100 to 100 (though the function
	 * saturates values above and below these values)/.
	 *
	 * @param motor, the Motor struct to be operated on.
	 * @param doot, the duty cycle to be set to.
	 *
	 */
	motor->duty_cycle = doot;
	// First, check if the motor is disabled
	if(motor->enable_flag != 1){
		// if the enable flag isn't set exit the function and do nothing.
		// we are also using != 1 so if there is a stray value in memory,
		// the motor doesn't accidentally enable.
		return;
	}

	// Next, saturate the duty cycle just in case.
	if(doot < -100){
		doot = -100;
	}
	if(doot > 100){
		doot = 100;
	}

	// We need to get the auto reload value for the timer we are using
	// signed value so we don't run into sign issues later
	int32_t ARR = (int32_t)(motor->timer->Init.Period + 1);

	// Now calculate the duty cycle in terms of the CCR value
	doot = doot*ARR/100;  // multiply first so we don't lose data

	// now we need to set the motors to the correct duty cycles
	// Forwards will be channels 1 and 3 for motors 1 and 2 respectively
	// Backwards will be channels 2 and 4 for motors 1 and 2 respectively


	// the below CCR's are based on the logic table of the toshiba, setting motor.
	// to brake mode
	// if duty cycle is <0
	if (doot < 0){
	    // check if it is the first or second motor.
		if(motor->channels == 1){
			motor->timer->Instance->CCR1 = ARR;
			motor->timer->Instance->CCR2 = ARR + doot;
		} else if(motor->channels == 2){
			motor->timer->Instance->CCR3 = ARR;
			motor->timer->Instance->CCR4 = ARR + doot;
		}else{
			// if neither return
			return;
		}
	// if duty cycle >=0
	} else{
		if(motor->channels == 1){
			motor->timer->Instance->CCR1 = ARR - doot;
			motor->timer->Instance->CCR2 = ARR;
		} else if(motor->channels == 2){
			motor->timer->Instance->CCR3 = ARR - doot;
			motor->timer->Instance->CCR4 = ARR;
		}else{
			return;
		}
	}
}


void motor_enable_disable(Motor* motor, uint8_t enable){
	/*
	 * @brief enables or disables motor based on user input
	 *
	 * @param motor, the Motor struct to act upot
	 * @param enable, the boolean of whether to enable or disable the motor
	 * with 1 being to enable and 0 being to disable.
	 */

	// if user wants to enable motor
	if(enable == 1){
		motor->enable_flag = 1;
		// First retrieve ARR to set motor to brake mode
		uint32_t ARR = (uint32_t)(motor->timer->Init.Period + 1);

		// Now set the correct motor pair to brake mode.
		if(motor->channels == 1){
			motor->timer->Instance->CCR1 = ARR;
			motor->timer->Instance->CCR2 = ARR;
		} else if(motor->channels == 2){
			motor->timer->Instance->CCR3 = ARR;
			motor->timer->Instance->CCR4 = ARR;
		}else{
			return;
		}

		// set the motor's enable flag to 1
		motor->enable_flag = 1;

	// if user wants to disable motor
	} else if(enable == 0){
		motor->enable_flag = 0;
		if(motor->channels == 1){
			motor->timer->Instance->CCR1 = 0;
			motor->timer->Instance->CCR2 = 0;
		} else if(motor->channels == 2){
			motor->timer->Instance->CCR3 = 0;
			motor->timer->Instance->CCR4 = 0;
		}else{
			return;
		}

		// set the motor's enable flag to 0
		motor->enable_flag = 0;
	}
}
