CC=gcc
CFLAGS=-c -Wall

all: image
	$(CC) image/obj/image.o  main.c -o app -lm -g

image: image/image.c
	mkdir image/obj
	$(CC) $(CFLAGS) image/image.c -o image/obj/image.o -g

clear:
	rm -r image/obj
	rm app