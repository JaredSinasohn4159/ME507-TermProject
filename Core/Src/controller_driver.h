/*
 * controller_driver.h
 *
 *  Created on: May 9, 2024
 *      Author: Jared Sinasohn
 */

#ifndef SRC_CONTROLLER_DRIVER_H_
#define SRC_CONTROLLER_DRIVER_H_


#include "stm32l4xx_hal.h"

/*
 * @struct Motor
 * @brief An implementation of a motor driver using a struct to emulate
 * Object Oriented Programming.  The motor has 4 parameters, timer which
 * indicates the timer to be used to run the motor, channels, which indicates
 * the channels to be used to run the motor, duty_cycle, the duty cycle to
 * run the motor at, and enable_flag, which determines if the motor is
 * allowed to run
 */
typedef struct {
    TIM_HandleTypeDef* timer; // Handle to the HAL timer object
    uint8_t  button; // which button is being pressed.  using channels 1-4 so 1 corresponds to 1 and 2 and 2 corresponds to 3 and 4
    uint32_t  period1; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint32_t  period2; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint32_t  re1; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint32_t  re2; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint32_t  fe1; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint32_t  fe2; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint8_t  fe_flag1; // flag that enables the motor
    uint8_t  fe_flag2; // flag that enables the motor
    int32_t  motor1_doot;
    int32_t  motor2_doot;
} RC_Controller;

void controller_driver_calc_per1(RC_Controller* controller);
void controller_driver_calc_per2(RC_Controller* controller);
int32_t controller_driver_calc_doot1(RC_Controller* controller);
int32_t controller_driver_calc_doot2(RC_Controller* controller);


#endif /* SRC_CONTROLLER_DRIVER_H_ */
