//controller.c

#include <stdio.h>
#include "controllerInterface.h"

int main()
{
    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID on windows - "\\\\.\\COM9"; 
    char BAUD[6] = "9600";          // Baud Rate
    int speed = 300;                // Servo speed

    initializeControllerWithSpeed(PORT, BAUD, speed);

    // goHome2(5);

    printf("\nEnter the cartesian pose values and press enter twice...\n");
    printf("\nHit 'q' to exit...\n");

    char buffer = 0;

    float x = 10;
    float y = 165;
    float z = 220;
    float pitch = -90;
    float roll = 0;

    while (buffer != 'q')
    {

        gotoPose(x, y, z, pitch, roll);
        grasp(0);

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
                if (x < 100 || x > -80)
                    x = arr[j];
                else
                    printf("out of range");
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
}
