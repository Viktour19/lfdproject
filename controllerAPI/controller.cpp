//controller.c

#include "controllerInterface.h"
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>


// #include <opencv2/opencv.hpp>
// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/video/video.hpp>


#define DEBUG 0
#define CALIBRATE 1
#define DEMO 0

// // Camera Index
// int idx = 0;

// // Camera Capture
// cv::VideoCapture cap;

struct timespec counter, start, pressKey, releaseKey;

// int* getObjectPose(InputArray frame)
// {
       
//     cv::Mat res;

// }

int open_port(void)
{
  int fd; /* File descriptor for the port */

  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd == -1)
  {
   // Could not open the port.          
   perror("open_port: Unable to open /dev/ttyUSB0 - ");
  }
  else
    fcntl(fd, F_SETFL, 0);

  struct termios options;

  tcgetattr(fd, &options); 
  //setting baud rates and stuff
  cfsetispeed(&options, B9600);
  cfsetospeed(&options, B9600);
  options.c_cflag |= (CLOCAL | CREAD);
  tcsetattr(fd, TCSANOW, &options);

  tcsetattr(fd, TCSAFLUSH, &options);

  options.c_cflag &= ~PARENB;//next 4 lines setting 8N1
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  //options.c_cflag &= ~CNEW_RTSCTS;

  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //raw input

  options.c_iflag &= ~(IXON | IXOFF | IXANY); //disable software flow control

  sleep(2); //required to make flush work, for some reason
  tcflush(fd,TCIOFLUSH);

  return (fd);
}


void sig(int s)
{
    spnav_close();
    exit(0);
}

int timediff(struct timespec end, struct timespec start)
{

    return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
}

//scale -350:350 into respective scales
float *scale_and_map(int m_x, int m_y, int m_z, int m_rx, int m_ry, int m_rz)
{
    float *poseDelta = malloc(sizeof(int) * 5); //should be 4. - x, y, z theta

    float f = (1.0 / 35.0);

    float x = f * m_x;
    float y = f * m_z;
    float z = f * m_y;
    float p = f * m_rx;
    float r = f * m_rz; // in the endeffector FOR, the roll is rotation about z when facing direction of wrist. when facing downward its rotation about x in the robot FOR, the roll is a rotation about y when in the wrist direction and about z when facing down

    // printf("\n \nMouse: \n %d %d %d %d %d %d \n", m_x, m_y, m_z, m_rx, m_ry, m_rz);
    // printf("Pose delta: \n %f %f %f %f %f \n", x, y, z, p, r);

    poseDelta[0] = x;
    poseDelta[1] = y;
    poseDelta[2] = z;
    poseDelta[3] = p;
    poseDelta[4] = r;

    return poseDelta;
}


int main()
{
    // // >>>>> Camera Settings
    // if (!cap.open(idx)) {
    //    cout << "Webcam not connected.\n" << "Please verify\n";
    //    //return EXIT_FAILURE;
    //    prompt_and_exit(1);
    // }

    // cap.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
    // cap.set(CV_CAP_PROP_FRAME_HEIGHT, 768);
    // // <<<<< Camera Settings


    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID on windows - "\\\\.\\COM9";
    char BAUD[6] = "9600";          // Baud Rate
    int speed = 300;                // Servo speed

    float x = 100;
    float y = 200;
    float z = 200;
    float pitch = -90;
    float roll = 0;
    int graspVal = 60;

    initializeControllerWithSpeed(PORT, BAUD, speed);

    goHome2(5);

#if DEMO

    spnav_event sev;
    signal(SIGINT, sig);

    // spnav_sensitivity(0.1f);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to the space navigator daemon\n");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while (spnav_wait_event(&sev))
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &counter);

        if (timediff(counter, start) > 200)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            if (sev.type == SPNAV_EVENT_MOTION)
            {
                float *poseDelta = scale_and_map(sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz);

                //take the max delta only and act on only that axis

                int index = 0;
                if(fabs(poseDelta[0]) < fabs(poseDelta[1])) index  = 1;
                if(fabs(poseDelta[index]) < fabs(poseDelta[2])) index = 2;
                if(fabs(poseDelta[index]) < fabs(poseDelta[3])) index = 3;
                if(fabs(poseDelta[index]) < fabs(poseDelta[4])) index = 4;
                
                int status = 0;
                if(index == 0)
                    status = gotoPose(x + poseDelta[0], y, z, pitch, roll);

                else if(index == 1)
                    status = gotoPose(x + poseDelta[0], y + poseDelta[1], z, pitch, roll);

                else if(index == 2)
                    status = gotoPose(x + poseDelta[0], y, z + poseDelta[2], pitch, roll);

                else if(index == 4)
                    status = gotoPose(x, y, z, pitch, roll + poseDelta[4]);

                //printf("\n status: %d \n", status );
                if (status)
                {
                    if(index == 0) x += poseDelta[0];
                    else if (index == 1) y += poseDelta[1];
                    else if (index == 2) z += poseDelta[2];
                    else if (index == 4) roll += poseDelta[4];
                
                }
            }
        }

            if (sev.type == SPNAV_EVENT_BUTTON)
            { /* SPNAV_EVENT_BUTTON */
                
                if(sev.button.bnum == 0 && sev.button.press)
                {
                    graspVal = abs(graspVal - 60);
                    
                    printf("%d\n", graspVal);
                    grasp(graspVal);
                }

                else if(sev.button.bnum == 1){

                    if(sev.button.press) clock_gettime(CLOCK_MONOTONIC_RAW, &pressKey);
                    else clock_gettime(CLOCK_MONOTONIC_RAW, &releaseKey);

                    //if it took at least 2 seconds to release, then end the demo
                    if (timediff(releaseKey, pressKey) > 2000)
                    {
                        break;
                    }
                }
            }
        
    }

    spnav_close();

#endif

//routine for manual input

#if DEBUG

    printf("\nEnter the cartesian pose values and press enter twice...\n");
    printf("\nHit 'q' to exit...\n");

    char buffer = 0;

     x = 100;
     y = 200;
     z = 200;
     pitch = -90;
     roll = 0;

    grasp(0);

    while (buffer != 'q')
    {
        gotoPose(x, y, z, pitch, roll);
        int i = 0;
        float arr[10];
        char temp;

        do
        {
            scanf("%f%c", &arr[i], &temp);
            i++;

        } while (temp != '\n');

        for (int j = 0; j < i; j++)
        {
            switch (j)
            {
            case 0:
                x = arr[j];
                break;

            case 1:
                y = arr[j];
                break;

            case 2:
                z = arr[j];
                break;

            case 3:
                pitch = arr[j];
                break;

            case 4:
                roll = arr[j];
                break;
            };
        }

        buffer = getchar();
    }

#endif


#if CALIBRATE


    sleep(10);

    int min_z = 100;
    int max_z = 270;
    int min_y = 130;
    int max_y = 240;
    int min_x = -90;
    int max_x = 75;

    for(int i = min_x; i< max_x; i+=20)
    {
        for(int j = min_y; j< max_y; j+=20)
        {
            gotoPose(i, j, min_z, pitch, roll);

            sleep(2);
        }
    }


#endif 

    return 0;
}
