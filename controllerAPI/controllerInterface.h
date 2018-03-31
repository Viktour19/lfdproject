//controllerinterface.h


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
 *  @param speed - servo speed
*/
void initializeControllerWithSpeed(char *port, char *baudrate, int speed);


/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
*/
void initializeController(char *port, char *baudrate);

/**
 * Set global speed of all servos
 */
void setSpeed(int speed);

/** 
 * set the positions of the servos connected to 
 * the controller to their default positions
 * @param numberOfServos 
 */
void goHome(int numberOfServos);

/** 
 * set the position of the specified servo to its default position
 * @param index
 */
void goServoHome(int index);

/**
 * Set specific servo pulse width
 * @param index, index of the servo
 * @param pw, pulse width
 */

void goHome2(int numberOfServos);

void setServoPW(int index, int pw);

/**
 * Set specific command
 * @param command, string to send to device - 
 * #channel Ppulsewidth Sspeed <carriage_return>
 */
void execute(char *command);

void executeCommand(int channel, int pos, int speed);

//inverse kinematics
int *getJointPositions(float x, float y, float z, float pitch_angle_d, float roll_angle_d);


void gotoPose(float x, float y, float z, float pitch, float roll);

void grasp(int d);