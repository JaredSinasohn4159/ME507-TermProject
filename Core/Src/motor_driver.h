/*
 * @file motor_driver.h
 * @date Apr 25, 2024
 * @author Jared Sinasohn
 */

#ifndef SRC_MOTOR_DRIVER_H_
#define SRC_MOTOR_DRIVER_H_

//include hal library
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
    uint8_t  channels; // which channels to use.  using channels 1-4 so 1 corresponds to 1 and 2 and 2 corresponds to 3 and 4
    int32_t  duty_cycle; // duty cycle of the motor from -ARR to ARR, which is 1000
    uint8_t  enable_flag; // flag that enables the motor
} Motor;

void motor_set_duty_cycle(Motor* motor, int32_t doot);
void motor_enable_disable(Motor* motor, uint8_t enable);


#endif /* SRC_MOTOR_DRIVER_H_ */
