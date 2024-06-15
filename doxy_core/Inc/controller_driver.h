/*!
 *  @file controller_driver.h
 *
 *  @date May 9, 2024
 *  @author Jared Sinasohn
 *  @brief  A pseudo-object-oriented structure for implementing the calculation of a pulse-width read from a two-channel rc controller.
 */

#ifndef SRC_CONTROLLER_DRIVER_H_
#define SRC_CONTROLLER_DRIVER_H_


#include "stm32l4xx_hal.h"

/*!
 * @class RC_Controller controller_driver.h
 *
 * @brief An implementation of an RC controller that sends pulse widths from its two control axes--a trigger and a wheel--through infrared.
 * The motor has parameters for the timer used for the input capture of the pulse widths, the pulse widths sent by the controller for each of the axes,
 * the values of time of the rising edges and falling axes for each axis, and flags that indicate a falling edge has been detected and therefore a
 * period should be caulculated.
 *
 * @param timer The microcontroller timer to use to run the input capture of the RC controller.  This is a timer pointer. The timer should be configured such that each count of the timer is read in microseconds, which in this case requires a prescaler of 79 and an auto-reload of 65535.
 * @param period1 The pulse width of the first axis
 * @param period2 The pulse width of the second axis
 * @param re1 Timer value on the rising edge captured on the first axis
 * @param re2 Timer value on the rising edge captured on the second axis
 * @param fe1 Timer value on the falling edge captured on the first axis
 * @param fe2 Timer value on the falling edge captured on the second axis
 * @param fe_flag1 Flag that is set by the input capture callback function to indicate a falling edge has been detected for the first axis.
 * @param fe_flag2 Flag that is set by the input capture callback function to indicate a falling edge has been detected for the first axis.
 */
typedef struct {
    TIM_HandleTypeDef* timer; // Handle to the HAL timer object
    uint32_t  period1; // pulse width of the first axis
    uint32_t  period2; // pulse width of the second axis
    uint32_t  re1; // Timer value on the rising edge captured on the first axis
    uint32_t  re2; // Timer value on the rising edge captured on the second axis
    uint32_t  fe1; // Timer value on the falling edge captured on the first axis
    uint32_t  fe2; // Timer value on the falling edge captured on the second axis
    uint8_t  fe_flag1; // flag that is set by the input capture callback function to indicate a falling edge has been detected for the first axis.
    uint8_t  fe_flag2; // flag that is set by the input capture callback function to indicate a falling edge has been detected for the first axis.
} RC_Controller;

/*! Calculates the pulse width sent by the first axis
 *
 * @param controller, the RC Controller instance to caculate and operate on
 *
 */
void controller_driver_calc_per1(RC_Controller* controller);

/*! Calculates the pulse width sent by the second axis
 *
 * @param controller, the RC Controller instance to caculate and operate on
 *
 */
void controller_driver_calc_per2(RC_Controller* controller);

#endif /* SRC_CONTROLLER_DRIVER_H_ */
