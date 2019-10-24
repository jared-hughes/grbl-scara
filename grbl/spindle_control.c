/*
  spindle_control.c - spindle control methods
  Part of Grbl
  Copyright (c) 2012-2015 Sungeun K. Jeon
  Copyright (c) 2009-2011 Simen Svale Skogsrud
  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"

/* RC-Servo PWM modification: switch between 0.6ms and 2.5ms pulse-width at 61Hz
   Prescaler 1024 = 15625Hz / 256Steps =  61Hz	64µs/step -> Values 15 / 32 for 1ms / 2ms
   Reload value = 0x07
   Replace this file in C:\Program Files (x86)\Arduino\libraries\GRBL
*/

#ifdef RC_SERVO
  #define RC_SERVO_SHORT     9//15       // Timer ticks for 0.6ms pulse duration  (9 for 0.6ms)
  #define RC_SERVO_LONG      39//32       // Timer ticks for 2.5 ms pulse duration  (39 for 2.5ms)
  //#define RC_SERVO_INVERT     1     // Uncomment to invert servo direction
#endif

void spindle_init()
{    
  // Configure variable spindle PWM and enable pin, if requried. On the Uno, PWM and enable are
  // combined unless configured otherwise.
  #ifdef VARIABLE_SPINDLE
    SPINDLE_PWM_DDR |= (1<<SPINDLE_PWM_BIT); // Configure as PWM output pin.
    #if defined(CPU_MAP_ATMEGA2560) || defined(USE_SPINDLE_DIR_AS_ENABLE_PIN)
      SPINDLE_ENABLE_DDR |= (1<<SPINDLE_ENABLE_BIT); // Configure as output pin.
    #endif     
  // Configure no variable spindle and only enable pin.
  #else  
    SPINDLE_ENABLE_DDR |= (1<<SPINDLE_ENABLE_BIT); // Configure as output pin.
  #endif
  
  #ifndef USE_SPINDLE_DIR_AS_ENABLE_PIN
    SPINDLE_DIRECTION_DDR |= (1<<SPINDLE_DIRECTION_BIT); // Configure as output pin.
  #endif
  spindle_stop();
}

#ifdef RC_SERVO
void spindle_stop()
{     // On the Uno, spindle enable and PWM are shared. Other CPUs have seperate enable pin.
       #ifdef RC_SERVO_INVERT
          OCR_REGISTER = RC_SERVO_LONG;
      #else
          OCR_REGISTER = RC_SERVO_SHORT;
      #endif
}

#else
void spindle_stop()
{
  // On the Uno, spindle enable and PWM are shared. Other CPUs have seperate enable pin.
  #ifdef VARIABLE_SPINDLE
    TCCRA_REGISTER &= ~(1<<COMB_BIT); // Disable PWM. Output voltage is zero.
    #if defined(CPU_MAP_ATMEGA2560) || defined(USE_SPINDLE_DIR_AS_ENABLE_PIN)
      #ifdef INVERT_SPINDLE_ENABLE_PIN
		SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);  // Set pin to high
	  #else
		SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT); // Set pin to low
	  #endif
    #endif
  #else
    #ifdef INVERT_SPINDLE_ENABLE_PIN
	  SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);  // Set pin to high
	#else
	  SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT); // Set pin to low
	#endif
  #endif
}
#endif

#ifdef RC_SERVO
void spindle_run(uint8_t direction, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }

  // Empty planner buffer to ensure spindle is set when programmed.
  protocol_auto_cycle_start();  //temp fix for M3 lockup
  protocol_buffer_synchronize();

  if (direction == SPINDLE_DISABLE) {

    spindle_stop();

  } else {
	#ifndef USE_SPINDLE_DIR_AS_ENABLE_PIN

    if (direction == SPINDLE_ENABLE_CW) {
      SPINDLE_DIRECTION_PORT &= ~(1<<SPINDLE_DIRECTION_BIT);
    } else {
      SPINDLE_DIRECTION_PORT |= (1<<SPINDLE_DIRECTION_BIT);
    }
	#endif

	#ifdef VARIABLE_SPINDLE

    // TODO: Install the optional capability for frequency-based output for servos.
    #define RC_SERVO_RANGE (RC_SERVO_LONG-RC_SERVO_SHORT)

	  #ifdef CPU_MAP_ATMEGA2560
      	TCCRA_REGISTER = (1<<COMB_BIT) | (1<<WAVE1_REGISTER) | (1<<WAVE0_REGISTER);
        TCCRB_REGISTER = (TCCRB_REGISTER & 0b11111000) | 0x07 | (1<<WAVE2_REGISTER) | (1<<WAVE3_REGISTER); // set to 1/1024 Prescaler
        OCR4A = 0xFFFF; // set the top 16bit value
        uint16_t current_pwm;
	  #else
        TCCRA_REGISTER = (1<<COMB_BIT) | (1<<WAVE1_REGISTER) | (1<<WAVE0_REGISTER);
        TCCRB_REGISTER = (TCCRB_REGISTER & 0b11111000) | 0x07; // set to 1/1024 Prescaler
	    uint8_t current_pwm;
	  #endif
    
    // clamp from 0 to 1
    if (rpm < 0) rpm = 0;
    if (rpm > 1) rpm = 1;
    // scale from MIN to MAX
    rpm = SPINDLE_MIN_RATIO + (SPINDLE_MAX_RATIO - SPINDLE_MIN_RATIO) * rpm;

    #ifdef RC_SERVO_INVERT
        current_pwm = floor( RC_SERVO_LONG - rpm*RC_SERVO_RANGE);
        OCR_REGISTER = current_pwm;
    #else
       current_pwm = floor( rpm*RC_SERVO_RANGE + RC_SERVO_SHORT);
        OCR_REGISTER = current_pwm;
    #endif
	  #ifdef MINIMUM_SPINDLE_PWM
        if (current_pwm < MINIMUM_SPINDLE_PWM) { current_pwm = MINIMUM_SPINDLE_PWM; }
	     OCR_REGISTER = current_pwm;
      #endif
    #endif
  }
}
#else
void spindle_run(uint8_t state, float rpm)
{
  if (sys.state == STATE_CHECK_MODE) { return; }
  protocol_buffer_synchronize(); // Empty planner buffer to ensure spindle is set when programmed.
  spindle_set_state(state, rpm);
}
#endif

#ifdef RC_SERVO
void spindle_set_state(uint8_t state, float rpm){
}
#else
void spindle_set_state(uint8_t state, float rpm)
{
  // Halt or set spindle direction and rpm.
  if (state == SPINDLE_DISABLE) {

    spindle_stop();

  } else {

    #ifndef USE_SPINDLE_DIR_AS_ENABLE_PIN
      if (state == SPINDLE_ENABLE_CW) {
        SPINDLE_DIRECTION_PORT &= ~(1<<SPINDLE_DIRECTION_BIT);
      } else {
        SPINDLE_DIRECTION_PORT |= (1<<SPINDLE_DIRECTION_BIT);
      }
    #endif

    #ifdef VARIABLE_SPINDLE
      // TODO: Install the optional capability for frequency-based output for servos.
      #ifdef CPU_MAP_ATMEGA2560
      	TCCRA_REGISTER = (1<<COMB_BIT) | (1<<WAVE1_REGISTER) | (1<<WAVE0_REGISTER);
        TCCRB_REGISTER = (TCCRB_REGISTER & 0b11111000) | 0x02 | (1<<WAVE2_REGISTER) | (1<<WAVE3_REGISTER); // set to 1/8 Prescaler
        OCR4A = 0xFFFF; // set the top 16bit value
        uint16_t current_pwm;
      #else
        TCCRA_REGISTER = (1<<COMB_BIT) | (1<<WAVE1_REGISTER) | (1<<WAVE0_REGISTER);
        TCCRB_REGISTER = (TCCRB_REGISTER & 0b11111000) | 0x02; // set to 1/8 Prescaler
        uint8_t current_pwm;
      #endif

      #define SPINDLE_RPM_RANGE (SPINDLE_MAX_RPM-SPINDLE_MIN_RPM)
      if ( rpm < SPINDLE_MIN_RPM ) { rpm = 0; }
      else {
        rpm -= SPINDLE_MIN_RPM;
        if ( rpm > SPINDLE_RPM_RANGE ) { rpm = SPINDLE_RPM_RANGE; } // Prevent integer overflow
      }
      current_pwm = floor( rpm*(PWM_MAX_VALUE/SPINDLE_RPM_RANGE) + 0.5);
      #ifdef MINIMUM_SPINDLE_PWM
        if (current_pwm < MINIMUM_SPINDLE_PWM) { current_pwm = MINIMUM_SPINDLE_PWM; }
      #endif
      OCR_REGISTER = current_pwm; // Set PWM pin output

      // On the Uno, spindle enable and PWM are shared, unless otherwise specified.
      #if defined(CPU_MAP_ATMEGA2560) || defined(USE_SPINDLE_DIR_AS_ENABLE_PIN)
        #ifdef INVERT_SPINDLE_ENABLE_PIN
          SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT);
        #else
          SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);
        #endif
      #endif

    #else
      #ifdef INVERT_SPINDLE_ENABLE_PIN
		SPINDLE_ENABLE_PORT &= ~(1<<SPINDLE_ENABLE_BIT);
	  #else
		SPINDLE_ENABLE_PORT |= (1<<SPINDLE_ENABLE_BIT);
	  #endif
    #endif

  }
}
#endif
