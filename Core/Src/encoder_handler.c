/*
 * encoder_handler.c
 *
 *  Created on: May 23, 2024
 *      Author: Jared Sinasohn
 */
#include "encoder_handler.h"

void encoder_read_curr_state(Encoder* encoder){
	encoder->prev_count = encoder->curr_count;
	encoder->prev_time = encoder->curr_time;
	encoder->curr_count = encoder->timer->Instance->CNT;
	encoder->curr_time = encoder->timing_timer->Instance->CNT;
	encoder->dx = delta(encoder->timer, encoder->prev_count,encoder->curr_count);
	int32_t dt = delta(encoder->timing_timer, encoder->prev_time,encoder->curr_time);
	encoder->pos = encoder->pos + encoder->dx;
	encoder->speed = encoder_calc_speed(encoder,encoder->dx,dt);

}
int32_t encoder_calc_speed(Encoder* encoder, int32_t dx,int32_t dt){
	if(dt == 0){
		return encoder->speed;
	}
	return ((dx)*1000000)/dt;
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

int32_t delta(TIM_HandleTypeDef* timer, uint32_t initial, uint32_t final){
	uint32_t ARR = (int32_t)(timer->Init.Period );
	int32_t overflow = ((ARR-1)/2)+1;
	int32_t delta = final-initial;
	if(delta >= overflow){
		delta = delta - overflow*2;
	}else if(delta <= -1*overflow){
		delta = delta + overflow;
	}
	return delta;
}



