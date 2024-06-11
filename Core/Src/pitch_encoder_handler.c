/*
 * pitch_encoder_handler.c
 *
 *  Created on: May 30, 2024
 *      Author: Jared Sinasohn
 */
#include "pitch_encoder_handler.h"

uint32_t get_pitch(PitchEncoder* p_enc){
	encoder_read_curr_state((p_enc->encoder));
	p_enc->delta = p_enc->pitch+p_enc->encoder->pos;
	p_enc->pitch += p_enc->delta;
	if(p_enc->delta != 0){
		p_enc->delta = p_enc->delta;
	}
	if(p_enc->pitch < 0){
		p_enc->pitch += 12;
		p_enc->encoder->timer->Instance->CNT = p_enc->pitch;
	}
	if(p_enc->pitch >= 12){
		p_enc->pitch -= 12;
		p_enc->encoder->timer->Instance->CNT = p_enc->pitch;
	}
	return p_enc->pitch;
}

