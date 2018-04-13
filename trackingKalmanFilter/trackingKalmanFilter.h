/* 
  Example use of openCV to track an object using colour segmentation and the Kalman filter
  ----------------------------------------------------------------------------------------
 
  (This is the interface file: it contains the declarations of dedicated functions to implement the application.
  These function are called by client code in the application file. The functions are defined in the implementation file.)

  David Vernon
  24 November 2017
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>

//opencv

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>


#define TRUE  1
#define FALSE 0
#define MAX_STRING_LENGTH   80
#define MAX_FILENAME_LENGTH 80
#define NUMBER_OF_POINTS_TO_DRAW 50 // in trace of located points

using namespace std;
using namespace cv;

/* function prototypes go here */

int trackingKalmanFilter(int min_hue, int max_hue, int min_sat, int max_sat); 

void prompt_and_exit(int status);
void prompt_and_continue();