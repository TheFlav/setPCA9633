## Makefile
# change to your prefered compiler
CC=gcc
# replace with name of the executable
OUTPUTNAME=setPCA9633
# add all source files here but with '.o' instead of '.c'
OBJS=PCA9633.o setPCA9633.o
default: all

all: $(OBJS)
	$(CC) -o $(OUTPUTNAME) $(OBJS)

debug: $(OBJS)
	$(CC) -g -o $(OUTPUTNAME) $(OBJS)

opt: $(OBJS)
	$(CC) -O3 -o $(OUTPUTNAME) $(OBJS)

.PHONY: clean
clean:
	rm *.o
	rm $(OUTPUTNAME)
