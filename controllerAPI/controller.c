#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "controllerinterface.h"


//inverse kinematics code authored by Prof. Vernon


/* Arm dimensions (mm). Standard AL5D arm */
#define D1       70     // Base height to X/Y plane  
#define A3       146.0  // Shoulder-to-elbow "bone"  
#define A4       187.0  // Elbow-to-wrist "bone"  
#define EZ  100         // Gripper length,  


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

    // If any servo ranges are exceeded, return an error
    //if (bas_pos < BAS_MIN || bas_pos > BAS_MAX || shl_pos < SHL_MIN || shl_pos > SHL_MAX || elb_pos < ELB_MIN || elb_pos > ELB_MAX || wri_pos < WRI_MIN || wri_pos > WRI_MAX)
    //     return NULL;
    
    // Position the servos
    /*Bas_Servo.writeMicroseconds(deg_to_us(bas_pos));
    Shl_Servo.writeMicroseconds(deg_to_us(shl_pos));
    Elb_Servo.writeMicroseconds(deg_to_us(elb_pos));
    Wri_Servo.writeMicroseconds(deg_to_us(wri_pitch_pos));
    Wro_Servo.writeMicroseconds(deg_to_us(wri_roll_pos));
    Gri_Servo.writeMicroseconds(deg_to_us(0));  // open
    */

    /* The rate is used to determine the delay period when iteratively writing intermediate servo positions  */
     /* between the current and the target value.  The interval is computed as 10*(10-rate) ms.               */
    /* Thus, a rate of 5 implies a delay of 50 ms between successive servo commands.                         */
 
    // actuateServo(1, bas_pos,       5);  
    // actuateServo(2, shl_pos,       5);  
    // actuateServo(3, elb_pos,       5);  
    // actuateServo(4, wri_pitch_pos, 9);  
    // actuateServo(5, wri_roll_pos,  9); 

    positions[0] = (int) bas_pos;
    positions[1] = (int) shl_pos;
    positions[2] = (int) elb_pos;
    positions[3] = (int) wri_pitch_pos;
    positions[4] = (int) wri_roll_pos;
    
    return positions;
}

void grasp(int d, int rate) {

  /* the grippers are approximately 30mm apart when open at servo angle of 0
   * and 0mm apart when closed at servo angle of 140
   * Thus, we close to approximately d mm with a servo angle of 140-4.7d
   */
  
   int angle;

   angle = (int) (150 - 4.6 * (float) d);
   angle = 120;
//    actuateServo(6, angle, rate);
   
}


int main() {
    
    char PORT[13] = "\\\\.\\COM9"; // USB Port ID
    char BAUD[6] = "9600"; // Baud Rate

    initializeControllerWithSpeed(PORT, BAUD, 300);

    // executeCommand("#0P2000 #1P2000 <CR>");
    
    //executeCommand("#0P750 #1P2000 <CR>");

    // executeCommand("#0P1500 #1P2000 #3P2000  S300 <CR>");
    
    // executeCommand("#0P2000 #1P1500 #3P1500 #4P1500 #5P1500 #6P1500 S300");
    
    //  goHome(7);

    zeroOffset[1] = 0;
    zeroOffset[2] = 0; 
    zeroOffset[3] = 0;
    zeroOffset[4] = 0;
    zeroOffset[5] = 0;
    zeroOffset[6] = 0;

    int* poss = getJointPositions(0, 187.0, 220.0, -90, 0); 

    printf("%d %d %d %d %d", poss[0], poss[1], poss[2], poss[3], poss[4]);
}

