rm controller
rm controller.o

sudo gcc -c controller.c
sudo gcc -o controller controller.c controllerInterface.c -lm

sudo ./controller