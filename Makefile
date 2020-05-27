CC=gcc
CFLAGS=-c -Wall

all: image_build
	$(CC) image/obj/image.o  main.c -o app -lm -g

image_build: | image/image.c
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image.c -o image/obj/image.o -g

clear:
	rm -r image/obj
	rm app
