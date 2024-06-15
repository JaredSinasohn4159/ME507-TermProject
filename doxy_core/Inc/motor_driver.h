/*!
 * @file motor_driver.h
 * @date Apr 25, 2024
 * @author Jared Sinasohn
 * @brief The header file for a generic DC motor driven from a standard h-bridge motor driver.
 */

#ifndef SRC_MOTOR_DRIVER_H_
#define SRC_MOTOR_DRIVER_H_

//include hal library
#include "stm32l4xx_hal.h"

/*!
 * @class Motor motor_driver.h
 *
 * @brief An implementation of a motor driver using a struct to emulate
 * Object Oriented Programming.  The motor has 4 parameters, timer which
 * indicates the timer to be used to run the motor, channels, which indicates
 * the channels to be used to run the motor, duty_cycle, the duty cycle to
 * run the motor at, and enable_flag, which determines if the motor is
 * allowed to run
 *
 * @param timer The microcontroller timer to use to control the pwm in the motor.  This is a timer pointer.
 * @param channels The channels the PWM signal should run on.  1 corresponds to 1 and 2, and 2 corresponds to 3 and 4.
 * @param duty_cycle The duty cycle of the motor from -ARR to ARR, which is 1000
 * @param enable_flag The flag which indicates whether or not the motor should be allowed to run.  A zero means the motor is disabled and a one means the motor is enabled.
 */
typedef struct {
    TIM_HandleTypeDef* timer; // Handle to the HAL timer object
    uint8_t  channels; // which channels to use.  using channels 1-4 so 1 corresponds to 1 and 2 and 2 corresponds to 3 and 4
    int32_t  duty_cycle; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint8_t  enable_flag; // flag that enables the motor
} Motor;

/*! This function implements the duty cycle setting of the motor.  It takes in
 * the motor struct and a duty cycle from -100 to 100 (though the function
 * saturates values above and below these values).
 *
 * @param motor, the Motor struct to be operated on.
 * @param doot, the duty cycle to be set to.
 *
 */
void motor_set_duty_cycle(Motor* motor, int32_t doot);

/*!
 * Enables or disables motor based on user input
 *
 * @param motor, the Motor struct to act upon
 * @param enable, the boolean of whether to enable or disable the motor
 * with 1 being to enable and 0 being to disable.
 */
void motor_enable_disable(Motor* motor, uint8_t enable);


#endif /* SRC_MOTOR_DRIVER_H_ */
