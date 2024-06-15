/*!
 * @file encoder_handler.c
 *
 * @date May 23, 2024
 * @author Jared Sinasohn
 * @brief This file implements the methods in the Encoder class.
 */
#include "encoder_handler.h"

/*!
 * This function reads the current encoder state and uses it to calculate the speed and position of the encoder.
 * @param encoder The encoder instance to operate on.
 * @attention If the timing_timer is set up correctly for the inputted encoder, this function should be called no faster than every microsecond.
 */
void encoder_read_curr_state(Encoder* encoder){
	//! set the previous times and previous counts to the previously current values
	encoder->prev_count = encoder->curr_count;
	encoder->prev_time = encoder->curr_time;
	//! get the count and time values from the two timers
	encoder->curr_count = encoder->timer->Instance->CNT;
	encoder->curr_time = encoder->timing_timer->Instance->CNT;
	//! calculate the difference between the current counts/times and previous counts/times using the delta function
	encoder->dx = delta(encoder->timer, encoder->prev_count,encoder->curr_count);
	encoder->dt = delta(encoder->timing_timer, encoder->prev_time,encoder->curr_time);
	//! set the encoder position to be the previous encoder position plus the delta position
	encoder->pos = encoder->pos + encoder->dx;
	//! set the speed of the encoder to the calculated value via encoder_calc_speed()
	encoder->speed = encoder_calc_speed(encoder,encoder->dx,encoder->dt);

}

/*!
 * This function calculates a speed based on a differential position and a differential time.
 * @param encoder The encoder instance to operate on
 * @param dx The differential position term in encoder counts.
 * @param dt The differential time term in microseconds
 * @return The calculated speed in counts per second
 */
int32_t encoder_calc_speed(Encoder* encoder, int32_t dx,int32_t dt){
	//! if the delta time is zero, return the previous speed to avoid divide by zero errors.
	if(dt == 0){
		return encoder->speed;
	}
	//! return the speed in counts/second knowing that dt is in microseconds
	return ((dx)*1000000)/dt;
}

/*!
 * This function zeros the encoder, including the timing_timer and timer timers.
 * @param encoder The encoder instance to operate on
 */
void zero(Encoder* encoder){
	encoder->timer->Instance->CNT = 0;
	encoder->timing_timer->Instance->CNT = 0;
	encoder->prev_count = 0;
	encoder->curr_count = 0;
	encoder->prev_time = 0;
	encoder->curr_time = 1;
	encoder->pos = 0;
	encoder->speed = 0;
}

/*!
 * This function calculates a delta between two values from a timer, while accounting for overflow.
 * @param timer The timer the initial and final values come from
 * @param initial The initial value of the timer
 * @param final The final value of the timer
 *
 * @returns delta The corrected delta value to account for overflow and underflow.
 */
int32_t delta(TIM_HandleTypeDef* timer, uint32_t initial, uint32_t final){
	//! get the auto reload value of the timer since it is the maximum value a timer can be
	uint32_t ARR = (int32_t)(timer->Init.Period );
	//! we can determine if something has overflowed or underflowed be assuming a delta will never be greater than half the auto reload value, which if the encoder is read enough is a good assumption
	int32_t overflow = ((ARR-1)/2)+1;
	//1 Calculate the delta
	int32_t delta = final-initial;
	//! if the value underflows, the delta will be a positive value greater than overflow, so just subtract off ARR+1 from the underflowed delta
	if(delta >= overflow){
		delta = delta - overflow*2;
	//! similarly, if the value overflows, the delta will be a negative value less than negative of overerflow, so add ARR+1 to the overflowed delta
	}else if(delta <= -1*overflow){
		delta = delta + overflow;
	}
	return delta;
}



