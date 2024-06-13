/*
 * pitch_encoder_handler.h
 *
 *  Created on: May 30, 2024
 *      Author: Jared Sinasohn
 */

#ifndef SRC_PITCH_ENCODER_HANDLER_H_
#define SRC_PITCH_ENCODER_HANDLER_H_

#include "stm32L4xx_hal.h"
#include "encoder_handler.h"

typedef struct{
	int16_t pitch;
	Encoder* encoder;
	int16_t delta;
}PitchEncoder;

uint32_t get_pitch(PitchEncoder* p_enc);

#endif /* SRC_PITCH_ENCODER_HANDLER_H_ */
