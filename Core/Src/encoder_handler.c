/*
 * encoder_handler.c
 *
 *  Created on: May 23, 2024
 *      Author: Jared Sinasohn
 */
#include "encoder_handler.h"

void encoder_read_curr_state(Encoder* encoder){
	uint32_t ARR = (int32_t)(encoder->timer->Init.Period );
	encoder->curr_count = encoder->timer->Instance->CNT;
	encoder->curr_time = HAL_GetTick();
	int32_t overflow = (ARR+1)/2;
	int32_t delta = encoder->curr_count - encoder->prev_count;
	if(delta >= overflow){
		delta = delta - overflow*2;
	}else if(delta <= -1*overflow){
		delta = delta + overflow;
	}else{
		delta = delta;
	}
	encoder->pos = encoder->pos + delta;
	encoder->speed = encoder_calc_speed(encoder,delta);
	encoder->prev_count = encoder->curr_count;
	encoder->prev_time = encoder->curr_time;

}
int32_t encoder_calc_speed(Encoder* encoder, int32_t delta){
	return (delta)*1000/((int32_t)(encoder->curr_time - encoder->prev_time));
}
void zero(Encoder* encoder){
	encoder->timer->Instance->CNT = 0;
	encoder->prev_count = 0;
	encoder->curr_count = 0;
	encoder->prev_time = HAL_GetTick();
	encoder->curr_time = HAL_GetTick();
	encoder->pos = 0;
	encoder->speed = 0;
}



