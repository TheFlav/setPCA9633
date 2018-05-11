all : setPCA9633 setPCA9633interactive

setPCA9633 : setPCA9633.c PCA9633.c PCA9633.h
	gcc -O3 -o setPCA9633 PCA9633.c setPCA9633.c

setPCA9633interactive : setPCA9633interactive.c PCA9633.c PCA9633.h
	gcc -O3 -o setPCA9633interactive setPCA9633interactive.c PCA9633.c

