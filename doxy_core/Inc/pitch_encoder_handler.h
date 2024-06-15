/*!
 * @file pitch_encoder_handler.h
 *
 * @date May 30, 2024
 * @author Jared Sinasohn
 */

#ifndef SRC_PITCH_ENCODER_HANDLER_H_
#define SRC_PITCH_ENCODER_HANDLER_H_

#include "stm32L4xx_hal.h"
#include "encoder_handler.h"

/*!
 * @class PitchEncoder pitch_encoder_handler.h
 *
 * @brief This class, in an object oriented sense, inherits the Encoder class but specifically reads values from the pitch encoder to map them to a specific pitch.
 *
 * @param pitch The current pitch the PitchEncoder read
 * @param encoder The base encoder the PitchEncoder uses (This is equivalent to inheriting the encoder class in an object-oriented language)
 * @param delta The difference between the previous reading of the encoder and the current reading of the encoder.
 */
typedef struct{
	int16_t pitch;
	Encoder* encoder;
	int16_t delta;
}PitchEncoder;

/*!
 * This function gets the current pitch based on the pitch selection knob
 * @param p_enc The pitch encoder object to read from
 * @return the current pitch, a number 0-11 mapped through the chromatic notes from A-Ab
 */
uint32_t get_pitch(PitchEncoder* p_enc);

#endif /* SRC_PITCH_ENCODER_HANDLER_H_ */
