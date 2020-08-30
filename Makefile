CC=gcc
CFLAGS=-c -Wall -lm -g

all: image_pipeline lecture grayscale binarized classification results

image/obj/image.o: image/image.c image/image.h
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image.c -o image/obj/image.o -g

image/obj/image_filters.o: image/image_filters.c image/image_filters.h
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image_filters.c -o image/obj/image_filters.o -g

utils/obj/utils.o: utils/utils.c utils/utils.h
	if [ ! -d "utils/obj" ]; then mkdir utils/obj; fi
	$(CC) $(CFLAGS) utils/utils.c -o utils/obj/utils.o -g

obj/image_pipeline.o: image_pipeline.c image_pipeline.h
	if [ ! -d "obj" ]; then mkdir obj; fi
	$(CC) $(CFLAGS) image_pipeline.c -o obj/image_pipeline.o -g

image_pipeline: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o main.c -o image_pipeline -lm -g

lecture: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o lecture.c -o lecture -lm -g
	
grayscale: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o grayScale_filter.c -o grayscale -lm -g

binarized: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o binarized.c -o binarized -lm -g

classification: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o classification.c -o classification -lm -g

results: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o results.c -o results -lm -g


 

clear:
	if [ -d image/obj ]; then rm -r image/obj; fi
	if [ -d utils/obj ]; then rm -r utils/obj; fi
	if [ -d obj ]; then rm -r obj; fi
	if [ -f image_pipeline ]; then rm image_pipeline; fi
	if [ -f lecture ]; then rm lecture; fi
	if [ -f grayscale ]; then rm grayscale; fi
	if [ -f binarized ]; then rm binarized; fi
	if [ -f classification ]; then rm classification; fi
	if [ -f results ]; then rm results; fi

