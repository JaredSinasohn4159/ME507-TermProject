/*
 * controller_driver.c
 *
 *  Created on: May 9, 2024
 *      Author: Jared Sinasohn
 */
#include "controller_driver.h"
void controller_driver_calc_per1(RC_Controller* controller){
	uint32_t per1 = controller->fe1-controller->re1;
	if(per1 < 2050 && per1 > 950){
		controller->period1 = per1;
	}
}
void controller_driver_calc_per2(RC_Controller* controller){
	uint32_t per2 = controller->fe2-controller->re2;
		if(per2 < 2050 && per2 > 950){
			controller->period2 = per2;
		}
}
int32_t controller_driver_calc_doot1(RC_Controller* controller){
	controller->motor1_doot = ((int32_t)(controller->period1)-1500)*100/500;
	return controller->motor1_doot;
}
int32_t controller_driver_calc_doot2(RC_Controller* controller){
	controller->motor2_doot = ((int32_t)(controller->period2)-1500)*100/500;
	return controller->motor2_doot;
}
