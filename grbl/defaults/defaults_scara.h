/*
  defaults_generic.h - defaults settings configuration file
  Part of Grbl

  Copyright (c) 2012-2015 Sungeun K. Jeon

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

/* The defaults.h file serves as a central default settings file for different machine
   types, from DIY CNC mills to CNC conversions of off-the-shelf machines. The settings
   here are supplied by users, so your results may vary. However, this should give you
   a good starting point as you get to know your machine and tweak the settings for your
   nefarious needs. */

#ifndef defaults_h
#define defaults_h

  // Grbl scara settings.
  #define MICROSTEPS_XY 4
  #define DEFAULT_UPPER_ARM 157.08 // mm
  #define DEFAULT_LOWER_ARM 183.45 // mm
  #define DEFAULT_X_STEPS_PER_MM 31.831*MICROSTEPS_XY // actually U steps/radian
  #define DEFAULT_Y_STEPS_PER_MM 31.831*MICROSTEPS_XY // actually L steps/radian
  #define DEFAULT_Z_STEPS_PER_MM 250.0
  #define DEFAULT_X_MAX_RATE 40.0 // rad/min
  #define DEFAULT_Y_MAX_RATE 40.0 // rad/min
  #define DEFAULT_Z_MAX_RATE 500.0 // mm/min
  #define DEFAULT_X_ACCELERATION (2.0*60*60) // 10*60*60 rad/min^2 = 10 rad/sec^2
  #define DEFAULT_Y_ACCELERATION (2.0*60*60) // 10*60*60 rad/min^2 = 10 rad/sec^2
  #define DEFAULT_Z_ACCELERATION (2.0*60*60) // 10*60*60 mm/min^2 = 10 mm/sec^2
  #define DEFAULT_X_MIN 20.0 // mm
  #define DEFAULT_R_MIN 100.0 // mm
  #define DEFAULT_R_MAX (DEFAULT_UPPER_ARM + DEFAULT_LOWER_ARM) // mm
  #define DEFAULT_X_OFFSET (DEFAULT_UPPER_ARM + DEFAULT_LOWER_ARM) // mm
  #define DEFAULT_Y_OFFSET 0.0 // mm
  #define DEFAULT_Z_OFFSET 0.0 // mm
  #define DEFAULT_STEP_PULSE_MICROSECONDS 10
  #define DEFAULT_STEPPING_INVERT_MASK 0
  #define DEFAULT_DIRECTION_INVERT_MASK 0
  #define DEFAULT_STEPPER_IDLE_LOCK_TIME 0 // msec (0-254, 255 keeps steppers enabled)
  #define DEFAULT_STATUS_REPORT_MASK ((BITFLAG_RT_STATUS_MACHINE_POSITION)|(BITFLAG_RT_STATUS_WORK_POSITION))
  #define DEFAULT_JUNCTION_DEVIATION 0.01 // mm
  #define DEFAULT_ARC_TOLERANCE 0.002 // mm
  #define DEFAULT_REPORT_INCHES 0 // false
  #define DEFAULT_INVERT_ST_ENABLE 0 // false
  #define DEFAULT_INVERT_LIMIT_PINS 0 // false
  #define DEFAULT_SOFT_LIMIT_ENABLE 1 // true
  #define DEFAULT_HARD_LIMIT_ENABLE 0  // false
  #define DEFAULT_HOMING_ENABLE 1  // false
  #define DEFAULT_HOMING_DIR_MASK 0 // move positive dir
  #define DEFAULT_HOMING_FEED_RATE 25.0 // mm/min
  #define DEFAULT_HOMING_SEEK_RATE 1.0 // mm/min
  #define DEFAULT_HOMING_DEBOUNCE_DELAY 250 // msec (0-65k)
  #define DEFAULT_HOMING_PULLOFF 1.0 // mm
  #define DEFAULT_SEGMENTATION_TOLERANCE 0.001 // mm
  #define DEFAULT_MM_PER_SEGMENT 2.0 // mm

#endif
