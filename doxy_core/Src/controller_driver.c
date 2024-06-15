/*!
 * @file controller_driver.c
 *
 * @date May 9, 2024
 * @author Jared Sinasohn
 * @brief This file implements the mehtods to calculate the period for the RC_Controller class.
 */
#include "controller_driver.h"

/*! Calculates the pulse width sent by the first axis.  This number, if the timer is set up correctly, should be a number
 *
 * @param controller, the RC Controller instance to caculate and operate on
 *
 */
void controller_driver_calc_per1(RC_Controller* controller){
	uint32_t per1 = controller->fe1-controller->re1;
	if(per1 < 2050 && per1 > 950){
		controller->period1 = per1;
	}
}
/*! Calculates the pulse width sent by the first axis.  This number, if the timer is set up correctly, should be a number
 *
 * @param controller, the RC Controller instance to caculate and operate on
 *
 */
void controller_driver_calc_per2(RC_Controller* controller){
	uint32_t per2 = controller->fe2-controller->re2;
		if(per2 < 2050 && per2 > 950){
			controller->period2 = per2;
		}
}
