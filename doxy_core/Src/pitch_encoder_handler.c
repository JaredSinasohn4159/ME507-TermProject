/*!
 * @file pitch_encoder_handler.c
 *
 * @date May 30, 2024
 * @author Jared Sinasohn
 * @brief Implements the methods of the PitchEncoder class.
 */
#include "pitch_encoder_handler.h"

/*!
 * This function gets the current pitch based on the pitch selection knob
 * @param p_enc The pitch encoder object to read from
 * @return the current pitch, a number 0-11 mapped through the chromatic notes from A-Ab
 */
uint32_t get_pitch(PitchEncoder* p_enc){
	//! read the current state of the encoder
	encoder_read_curr_state((p_enc->encoder));
	//! store the delta of the encoder
	p_enc->delta = (int16_t)(p_enc->encoder->dx);
	//! add the delta to the pitch
	p_enc->pitch += p_enc->delta;
	//! if the pitch hasn't changed, just return the pitch
	if(p_enc->delta == 0){
		return p_enc->pitch;
	}
	//! we can treat the pitch as a number between 0 and 11, which can underflow and overflow.  We can run a similar algorithm to the delta() function in the encoder class to correct for this.
	if(p_enc->pitch < 0){
		p_enc->pitch += 12;
	}
	if(p_enc->pitch >= 12){
		p_enc->pitch -= 12;
	}
	return p_enc->pitch;
}

