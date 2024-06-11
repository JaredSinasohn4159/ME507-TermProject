/*
 * Encoder.h
 *
 *  Created on: May 18, 2024
 *      Author: Jared Sinasohn
 */

#ifndef SRC_ENCODER_HANDLER_H_
#define SRC_ENCODER_HANDLER_H_

#include "stm32L4xx_hal.h"


/*
 * @class Encoder
 * @brief lorem ipsum...
 */
typedef struct{
	TIM_HandleTypeDef* timer;
	TIM_HandleTypeDef* timing_timer;
	uint32_t prev_count;
	uint32_t curr_count;
	uint32_t prev_time;
	uint32_t curr_time;
	int32_t pos;
	int32_t speed;
}Encoder;

void encoder_read_curr_state(Encoder* encoder);
int32_t encoder_calc_speed(Encoder* encoder,int32_t dx, int32_t dt);
void zero(Encoder* encoder);
int32_t delta(TIM_HandleTypeDef* timer, uint32_t initial, uint32_t final);

#endif /* SRC_ENCODER_HANDLER_H_ */
