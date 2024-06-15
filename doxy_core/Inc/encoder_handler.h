/*!
 * @file encoder_handler.h
 *
 * @date May 18, 2024
 * @author Jared Sinasohn
 * @brief The header file for a class that implements a generic quadrature encoder
 */

#ifndef SRC_ENCODER_HANDLER_H_
#define SRC_ENCODER_HANDLER_H_

#include "stm32L4xx_hal.h"


/*!
 * @class Encoder encoder_handler.h
 *
 * @brief A generic implementation of a quadrature encoder that tracks the position of the encoder in counts and the speed the encoder is traveling at in counts per second.
 *
 * @param timer The timer the encoder channels are on.
 * @param timing_timer The timer that is used to track time in order to calculate speed accurately.
 * @attention The timing_timer must be configures such that it counts values in microseconds
 * @param prev_count The previous count timer read.
 * @param curr_count The current count timer reads.
 * @param prev_time The previous count timing_timer read.
 * @param curr_time The current count timing_timer reads.
 * @param pos The position of the encoder in counts.
 * @param speed The speed the motor is traveling at in counts per second
 * @param dx The difference in position between the previous encoder reading and the current encoder reading in counts
 * @param dx The difference in time between the previous encoder reading and the current encoder reading in microseconds
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
	int32_t dx;
	int32_t dt;
}Encoder;

/*!
 * This function reads the current encoder state and uses it to calculate the speed and position of the encoder.
 * @param encoder The encoder instance to operate on.
 * @attention If the timing_timer is set up correctly for the inputted encoder, this function should be called no faster than every microsecond.
 */
void encoder_read_curr_state(Encoder* encoder);

/*!
 * This function calculates a speed based on a differential position and a differential time.
 * @param encoder The encoder instance to operate on
 * @param dx The differential position term in encoder counts.
 * @param dt The differential time term in microseconds
 * @return The calculated speed in counts per second
 */
int32_t encoder_calc_speed(Encoder* encoder,int32_t dx, int32_t dt);

/*!
 * This function zeros the encoder, including the timing_timer and timer timers.
 * @param encoder The encoder instance to operate on
 */
void zero(Encoder* encoder);

/*!
 * This function calculates a delta between two values from a timer, while accounting for overflow.
 * @param timer The timer the initial and final values come from
 * @param initial The initial value of the timer
 * @param final The final value of the timer
 *
 * @returns delta The corrected delta value to account for overflow and underflow.
 */
int32_t delta(TIM_HandleTypeDef* timer, uint32_t initial, uint32_t final);

#endif /* SRC_ENCODER_HANDLER_H_ */
