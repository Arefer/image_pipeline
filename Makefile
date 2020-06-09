CC=gcc
CFLAGS=-c -Wall

all: image_build utils-build
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o image_pipeline.c -o app -lm -g

image_build: | image/image.c
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image.c -o image/obj/image.o -g
	$(CC) $(CFLAGS) image/image_filters.c -o image/obj/image_filters.o -g

utils-build: | utils/utils.c
	if [ ! -d "utils/obj" ]; then mkdir utils/obj; fi
	$(CC) $(CFLAGS) utils/utils.c -o utils/obj/utils.o -g

clear:
	rm -r image/obj
	rm app
