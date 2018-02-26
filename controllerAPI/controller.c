//controller.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "controllerInterface.h"


//inverse kinematics code authored by Prof. Vernon


/* Arm dimensions (mm). Standard AL5D arm */
#define D1       70     // Base height to X/Y plane  
#define A3       146.0  // Shoulder-to-elbow "bone"  
#define A4       187.0  // Elbow-to-wrist "bone"  
#define EZ  100         // Gripper length
#define MIN_PW 750 //lowest pulse width
#define MAX_PW 2250 //highest pulsewidth
#define DEG_PW 0.09 //degrees to pulse width conversation factor


int zeroOffset[7];

float degrees(float radians)
{
    float degrees = radians * 180.0 / M_PI;
    return degrees;
}

float radians(float degrees)
{
    float radians = degrees/(180.0 / M_PI);
    return radians;
}

int* getJointPositions(float x, float y, float z, float pitch_angle_d, float roll_angle_d) {
  
    int* positions = malloc(sizeof(int) * 6);
    
    float hum_sq;
    float uln_sq;
    float wri_roll_angle_d;
    float bas_pos;
    float shl_pos;  
    float elb_pos; 
    float wri_pitch_pos;
    float wri_roll_pos;

    hum_sq = A3 * A3;
    uln_sq = A4 * A4;

    /* the servo angles corresponding to joint angles of zero */
    
    zeroOffset[1] = 90;
    zeroOffset[2] = 140; 
    zeroOffset[3] = 90;
    zeroOffset[4] = 90;
    zeroOffset[5] = 90;
    zeroOffset[6] = 0;
    
    //grip angle in radians for use in calculations
    float pitch_angle_r = radians(pitch_angle_d);    
    float roll_angle_r =  radians(roll_angle_d);   
      
    // Base angle and radial distance from x,y coordinates
    float bas_angle_r = atan2(x, y);
    float bas_angle_d = degrees(bas_angle_r);
    
    float rdist = sqrt((x * x) + (y * y));
  
    // rdist is y coordinate for the arm
    y = rdist;  //DV BAD PRACTICE IN ORIGINSAL CODE: OVERWRITING A PARAMETER!
    
    // Wrist position
    float wrist_z = z - D1;
    float wrist_y = y;
    
    // Shoulder to wrist distance (AKA sw)
    float s_w = (wrist_z * wrist_z) + (wrist_y * wrist_y);
    float s_w_sqrt = sqrt(s_w);

    
    // s_w angle to ground
    float a1 = atan2(wrist_z, wrist_y);
    
    // s_w angle to A3
    float a2 = acos(((hum_sq - uln_sq) + s_w) / (2 * A3 * s_w_sqrt));
 
    // Shoulder angle
    float shl_angle_r = a1 + a2;
    // If result is NAN or Infinity, the desired arm position is not possible
    if (isnan(shl_angle_r) || isinf(shl_angle_r))
        return NULL;
    float shl_angle_d = degrees(shl_angle_r);

    float a1_d = degrees(a1);
    float a2_d = degrees(a2);

    // Elbow angle
    float elb_angle_r = acos((hum_sq + uln_sq - s_w) / (2 * A3 * A4));
    // If result is NAN or Infinity, the desired arm position is not possible
    if (isnan(elb_angle_r) || isinf(elb_angle_r))
        return NULL;
    float elb_angle_d = degrees(elb_angle_r);
    float elb_angle_dn = -(180.0 - elb_angle_d);
    
    // Wrist angles
    float wri_pitch_angle_d = (pitch_angle_d - elb_angle_dn) - shl_angle_d;
    
    if (((int) pitch_angle_d == -90) || ((int) pitch_angle_d == 90)) {
 
      /* special case: we can adjust the required roll to compensate for the base rotation */
      
      wri_roll_angle_d  =  roll_angle_d - bas_angle_d;
    }
    else {
      wri_roll_angle_d  =  roll_angle_d;
    }
    
 
    // Calculate servo angles
    // Calc relative to servo midpoint to allow compensation for servo alignment
    /*
    bas_pos = BAS_MID + degrees(bas_angle_r);
    shl_pos = SHL_MID + (shl_angle_d - 90.0);  
    elb_pos = ELB_MID - (elb_angle_d - 90.0); 
    wri_pitch_pos = WRI_MID + wri_pitch_angle_d;
    wri_roll_pos  = WRO_MID + wri_roll_angle_d;
    */
    
    bas_pos       =    bas_angle_d         + zeroOffset[1];
    shl_pos       =   (shl_angle_d - 90.0) + zeroOffset[2];
    elb_pos       = - (elb_angle_d - 90.0) + zeroOffset[3];
    wri_pitch_pos =    wri_pitch_angle_d   + zeroOffset[4];
    wri_roll_pos  =    wri_roll_angle_d    + zeroOffset[5];       
  
//#define DEBUG  // we use conditional compilation to avoid compiling unused code
#ifdef DEBUG
    printf("X: ");
    printf(x);
    printf("  Y: ");
    printf(y);             // DV THIS HAS BEEN OVERWRITTEN IN ORIGINAL CODE
    printf("  Z: ");
    printf(z);
    printf("  pitch: ");
    printf(pitch_angle_d);
    printf("  roll: ");
    printf(roll_angle_d);
    printfln();
    printf("Base Pos: ");
    printf(bas_pos);
    printf("  Shld Pos: ");
    printf(shl_pos);
    printf("  Elbw Pos: ");
    printf(elb_pos);
    printf("  Pitch Pos: ");
    printfln(wri_pitch_pos);
    printf("  Roll Pos: ");
    printfln(wri_roll_pos);
    printf("bas angle: ");
    printf(degrees(bas_angle_r));  
    printf("  shl angle: ");
    printf(shl_angle_d);  
    printf("  elb angle: ");
    printf(elb_angle_d);
    printf("  Pitch d: ");
    printfln(wri_pitch_angle_d);
    printf("  Roll d: ");
    printfln(wri_roll_angle_d);
    printfln();

#endif 

    //convert positions in degrees to pulse width (0.09 degrees per unit pulse width)

    positions[0] = (int) (bas_pos / DEG_PW);
    positions[1] = (int) (shl_pos / DEG_PW);
    positions[2] = (int) (elb_pos / DEG_PW);
    positions[3] = (int) (wri_pitch_pos / DEG_PW);
    positions[4] = (int) (wri_roll_pos / DEG_PW);
    
    //scale pw to be within range

    positions[0] = positions[0] + MIN_PW;
    positions[1] = positions[1] + MIN_PW;
    positions[2] = positions[2] + MIN_PW;
    positions[3] = positions[3] + MIN_PW;
    positions[4] = positions[4] + MIN_PW;
    
    return positions;
}

int grasp(int d) {

  /* the grippers are approximately 30mm apart when open at servo angle of 0 
   * and 0mm apart when closed at servo angle of 140 (500 PW - 1555 PW )
   * Thus, we close to approximately d mm with a servo angle of 140-4.7d 
   * or PW of 1553-35.2d
   */
  
   int pw, angle;

   angle = (int) (150 - 4.6 * (float) d);
   pw =  (int) (500 + 35.17 * (float) d);

   return pw;
}


int main() {
    
    // char PORT[13] = "\\\\.\\COM9"; // USB Port ID

    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID    

    char BAUD[6] = "9600"; // Baud Rate

    initializeControllerWithSpeed(PORT, BAUD, 300);

    int* poss = getJointPositions(0, 187.0, 220.0, -90, 0);

    printf("%d %d %d %d %d \n", poss[0], poss[1], poss[2], poss[3], poss[4]);

    char command[100];

    sprintf(command, "#0P%d <CR>", poss[0]);

    executeCommand(command);

    sprintf(command, "#1P%d <CR>", poss[1]);

    executeCommand(command);

    sprintf(command, "#2P%d <CR>", poss[2]);

    executeCommand(command);

    sprintf(command, "#3P%d <CR>", poss[3]);

    executeCommand(command);

    sprintf(command, "#4P%d <CR>", poss[4]);

    executeCommand(command);
    
    //  goHome(7);

}

