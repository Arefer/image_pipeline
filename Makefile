CC=gcc
CFLAGS=-c -Wall -lm -g

all: bin/save bin/nearly_black bin/binarize bin/gray_scale bin/load_image image_pipeline

image/obj/image.o: image/image.c image/image.h
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image.c -o image/obj/image.o

image/obj/image_filters.o: image/image_filters.c image/image_filters.h
	if [ ! -d "image/obj" ]; then mkdir image/obj; fi
	$(CC) $(CFLAGS) image/image_filters.c -o image/obj/image_filters.o

utils/obj/utils.o: utils/utils.c utils/utils.h
	if [ ! -d "utils/obj" ]; then mkdir utils/obj; fi
	$(CC) $(CFLAGS) utils/utils.c -o utils/obj/utils.o

obj/image_pipeline.o: image_pipeline.c image_pipeline.h
	if [ ! -d "obj" ]; then mkdir obj; fi
	$(CC) $(CFLAGS) image_pipeline.c -o obj/image_pipeline.o

bin/load_image: image/load_image.c image/obj/image.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	$(CC) image/obj/image.o image/load_image.c -o bin/load_image -g -lm

bin/gray_scale: image/gray_scale.c image/obj/image.o image/obj/image_filters.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	$(CC) image/obj/image.o image/obj/image_filters.o image/gray_scale.c -o bin/gray_scale -g -lm

bin/binarize: image/binarize.c image/obj/image.o image/obj/image_filters.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	$(CC) image/obj/image.o image/obj/image_filters.o image/binarize.c -o bin/binarize -g -lm

bin/nearly_black: image/nearly_black.c image/obj/image.o image/obj/image_filters.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	$(CC) image/obj/image.o image/obj/image_filters.o image/nearly_black.c -o bin/nearly_black -g -lm

image_pipeline: image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o main.c
	$(CC) image/obj/image.o image/obj/image_filters.o utils/obj/utils.o obj/image_pipeline.o main.c -o image_pipeline -lm -g

bin/save: image/save.c image/obj/image.o image/obj/image_filters.o obj/image_pipeline.o
	if [ ! -d "bin" ]; then mkdir bin; fi
	$(CC) image/obj/image.o image/obj/image_filters.o obj/image_pipeline.o image/save.c -o bin/save -g -lm

clear:
	if [ -d image/obj ]; then rm -r image/obj; fi
	if [ -d utils/obj ]; then rm -r utils/obj; fi
	if [ -d obj ]; then rm -r obj; fi
	if [ -f image_pipeline ]; then rm image_pipeline; fi
	if [ -d bin ] ; then rm -r bin; fi

