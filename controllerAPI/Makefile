CC = g++
CFLAGS = -pedantic -Wall -g -I../.. -I/usr/local/include
LDFLAGS = -L../.. -L/usr/local/lib -lspnav -lX11 controllerInterface.c -lm

.PHONY: all
all: controller

controller: controller.c
	$(CC) $(CFLAGS) -DBUILD_AF_UNIX -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -f controller
