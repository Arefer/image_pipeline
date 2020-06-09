CC=gcc
CFLAGS=-c -Wall -lm -g

all: image_pipeline

image/obj/image.o: image/image.c image/image.h
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image.c -o image/obj/image.o -g

image/obj/image_filters.o: image/image_filters.c image/image_filters.h
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image_filters.c -o image/obj/image_filters.o -g

utils/obj/utils.o: utils/utils.c utils/utils.h
	if [ ! -d "utils/obj" ]; then mkdir utils/obj; fi
	$(CC) $(CFLAGS) utils/utils.c -o utils/obj/utils.o -g

image_pipeline: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o image_pipeline.c -o image_pipeline -lm -g

clear:
	if [ -d image/obj ]; then rm -r image/obj; fi
	if [ -d utils/obj ]; then rm -r utils/obj; fi
	if [ -f image_pipeline ]; then rm image_pipeline; fi
