
//todo
//validation
//memory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "controllerinterface.h"

//Helper function prototypes

void fail(char *message);
int getTimeNeeded(int idx, int destination, int speed);
int getDistance(int idx, int destination);
int getPW(int idx);

#define DEFAULT_SLEEP_TIME 3
#define COMMAND_SIZE 200
#define MAX_SERVOS 32
#define MIN_PW 750 //lowest pulse width
#define MAX_PW 2250 //highest pulsewidth

char PORT[20];
char BAUD[6];
int ACTIVE_SERVOS_COUNT;
int SPEED;
int MESASGE_LENGTH = 500;


int defaultPulseWidths[32] =  {2000, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
int currentPulseWidths[32] =   {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
 *  @param speed - servo speed
*/
void initializeControllerWithSpeed(char *port, char *baudrate, int speed)
{
    strcpy(PORT, port);
    strcpy(BAUD, baudrate);
    SPEED = speed;
}


/**
 * Set global speed of all servos
 */
void setSpeed(int speed) {
    SPEED = speed;
}


/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
*/
void initializeController(char *port, char *baudrate)
{
    strcpy(PORT, port);
    strcpy(BAUD, baudrate);
}


/** 
 * set the positions of the servos connected to 
 * the controller to their default positions
 * @param numberOfServos 
 */
void goHome(int numberOfServos)
{
    if(numberOfServos > MAX_SERVOS)
    {
        char message[MESASGE_LENGTH];
        sprintf(message, "Maximum number of servos is %d", MAX_SERVOS);
        fail(message);
    }

    ACTIVE_SERVOS_COUNT =  numberOfServos;

    char commandbuilder[COMMAND_SIZE] = "";
    char command[COMMAND_SIZE];
    
    for(int i = 0; i < ACTIVE_SERVOS_COUNT; i++)
    {
        char temp[COMMAND_SIZE];

        //#channel Ppulsewidth Sspeed <carriage_return>

        sprintf(temp, "#%dP%d", i, defaultPulseWidths[i]);
        
        strcat(commandbuilder, temp);
        strcat(commandbuilder, " ");                

        currentPulseWidths[i] = defaultPulseWidths[i];
   
    }
    char temp[COMMAND_SIZE];

    sprintf(temp, "S%d <CR>", SPEED);
    
    strcat(commandbuilder, temp);  
    
    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);
    printf(commandbuilder);
    system(command);
    sleep(DEFAULT_SLEEP_TIME);
    
}

/** 
 * set the position of the specified servo to its default position
 * @param index
 */
void goServoHome(int index)
{
    if(index >= ACTIVE_SERVOS_COUNT)
    {
        char message[MESASGE_LENGTH];
        sprintf(message, "Servo not active");
        fail(message);
    }

    char commandbuilder[COMMAND_SIZE];
    char command[COMMAND_SIZE];
    
    //#channel Ppulsewidth Sspeed <carriage_return>

    sprintf(commandbuilder, "#%dP%dS%d<CR>", index, defaultPulseWidths[index], SPEED);
    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);

    system(command);
    currentPulseWidths[index] = defaultPulseWidths[index];
    
    sleep(DEFAULT_SLEEP_TIME);
}


/**
 * Set specific servo pulse width
 * @param index, index of the servo
 * @param pw, pulse width
 */

void setServoPW(int index, int pw)
{
    if(index >= ACTIVE_SERVOS_COUNT)
    {
        char message[MESASGE_LENGTH];
        sprintf(message, "Servo not active");
        fail(message);
    }

    if(pw < MIN_PW || pw > MAX_PW)
    {
        char message[MESASGE_LENGTH];
        sprintf(message, "Pulse width not in range");
        fail(message);
    }
    
    char commandbuilder[COMMAND_SIZE];
    char command[COMMAND_SIZE];

    //#channel Ppulsewidth Sspeed <carriage_return>

    sprintf(commandbuilder, "#%dP%dS%d<CR>", index, pw, SPEED);
    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);

    system(command);
    sleep(getTimeNeeded(index, pw, SPEED));

    currentPulseWidths[index] = pw;

} 


/**
 * Set specific command
 * @param command, string to send to device - 
 * #channel Ppulsewidth Sspeed <carriage_return>
 */
void executeCommand(char *command)
{

    if(sizeof(command) < 6)
    {
        char message[MESASGE_LENGTH];
        sprintf(message, "Invalid command. Should be of size %d at least", 6);
        fail(message);
    }
    else
    {
        char execcommand[sizeof(command) + 20];

        sprintf(execcommand, "echo \"%s\" > %s", command, PORT);
        system(execcommand);
    }
}



//helper methods

/**
 * Print failure message and return
 * @param message, output message
 */
void fail(char *message) {
    printf("%s\n", message);
    exit(1);
}

/**
 * Gets the time needed to complete the movement
 * Time = Distance / Speed
 * @param idx, index of the servo
 * @param destination, destination pulse width
 * @param speed, the speed being used
 */
int getTimeNeeded(int idx, int destination, int speed) {
    int neededTime;
    int distance = getDistance(idx, destination);
    neededTime = distance / speed;
    return neededTime;   
}

/**
 * Gets the distance between the current position and the destination
 * @param idx, index of the servo
 * @param destination, destination pulse width
 */
int getDistance(int idx, int destination) {
    int distance = abs(getPW(idx) - destination);
    return distance;
}

/**
 * Get pulse width of specified servo
 * @param idx, index of the servo
 */
int getPW(int idx) {
   return currentPulseWidths[idx];
}



// method to recieve catesian coordinates and translate to joint positions (pulse widths)

//inverse kinematics routine