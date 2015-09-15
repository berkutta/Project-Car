/**
\file     motor_management/servo_lib.h
\ingroup  servo_lib
\brief    Implementation of the Servo control.
\author   Benjamin Marty
*/


#ifndef SERVO_LIB_H
#define SERVO_LIB_H

/** \brief Init Function for Servo 1
*/
void init_servo_1( void );

/** \brief Control Function to position the Servo

  \param Servo position in grad (0-180)
*/
void control_servo_1( char grad );

#endif /* SERVO_LIB_H */