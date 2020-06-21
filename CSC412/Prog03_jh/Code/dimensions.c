/**
 *	@file dimensions.c
 *
 *	@brief This is a program to read the dimensions of an image.
 *	This program takes as arguments up to three optional arguments and 
 *	an input file. It reads the image data and prints the dimensions
 *	according to the input arguments.
 *	
 *	@author Jesse Hazard
 *
 *	@date 10/13/2019
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "image_IO_TGA.h"

//declare test function
unsigned int test(char *arg);


/**	
*	
*	@brief Main function
*	The main function takes a file path to a .tga file and up to two of 
*	three options as arguments. Then it reads the data from the file and 
*	prints the dimensions according to the desired output.
*	@param argc An integer representing the number of arguments
*	@param arv[] The array of arguments in char* form 
*	@return 0 if successfully complete
*
*/ 
int main(int argc, char** argv) {

	
	FILE *log = fopen("dimensions_log.txt","a");/**< dimensions' log file. */
	fprintf(log, "dimensions called.\n");


	//Validate argument count
	if (argc > 4){
		printf("dimension recieved too many arguments.\n");
		fprintf(log, "dimensions closed due to too many arguments.\n");
		fclose(log);
		exit(0);
	} else if (argc < 2) {
		printf("dimension recieved no arguments.\n");
		fprintf(log, "dimensions closed due to too few arguments.\n");
		fclose(log);
		exit(0);
	}

	//validate file
	char	*fname = argv[argc-1]; /**< The input file path to be verified */
	int hold = access(fname, F_OK);
	
	if (hold == -1) {
		printf("dimension's last argument not a valid file path.\n");
		fprintf(log, "dimensions closed because %s is an invalid file path.\n", fname);
		exit(0);
	} else if (strcmp(&fname[strlen(fname)-4], ".tga") != 0) {
		printf("dimension's path not a .tga file.\n");
		fprintf(log, "dimensions closed because %s is not a tga image file.\n", fname);
		fclose(log);
		exit(0);
	}
	
	//Create program variables
	//arguments[0] = -v = verbose
	//arguments[1] = -h = height
	//arguments[2] = -w = width
	unsigned int	arguments[3] = { 0 };/**< The array representing optional input arguments. */
	unsigned int	arg = 0;
	FILE		*tga_in;/**< The open input file to be tested. */
	unsigned int	nbRows, nbCols;
	
	//if an option was entered
	if (argc > 2) {

		//test if valid arg
		arg = test(argv[1]);
		if (arg == 4) {
			printf("dimensions only accepts the following options:\n-v, -h, -w\n");
			fprintf(log, "dimensions closed because %s is an invalid option.\n", argv[1]);
			fclose(log);
			exit(0);
		} else arguments[arg] += 1;

	//if a second option was entered
	} if (argc == 4) {

		//test if valid or repeat arg
		arg = test(argv[2]);
		if (arg == 4) {
			printf("dimensions only accepts the following options:\n-v, -h, -w\n");
			fprintf(log, "dimensions closed because %s is an invalid option.\n", argv[2]);
			fprintf(log, "\n");
			fclose(log);
			exit(0);
		} else if (arguments[arg] == 1) {
			printf("dimensions recieved the same option twice\n");
			fprintf(log, "dimensions closed because both optional arguments were identical.\n");
			fclose(log);
			exit(0);
		} else arguments[arg] += 1;

		//test if exclusive args
		if (arguments[1] == arguments[2]) {
			printf("dimension options -h and -w are mutually exclusive\n");
			
			fprintf(log, "dimensions closed because both -h and -w were given as optional arguments.\n");
			fclose(log);
			exit(0);
	}	}

	//test if file has content
	tga_in = fopen(argv[argc-1], "rb" );
	if (tga_in == NULL) {
		printf("Cannot open image file\n");
		fprintf(log, "dimensions closed because the image file was empty.\n");
		fclose(log);	
		exit(1);
	} else {
		fseek (tga_in, 0, SEEK_END);
		int size = ftell(tga_in);
		if (size == 0) {
			printf("%s is empty", argv[argc-1]);
			fprintf(log, "dimensions closed because the image file was empty.\n");
			fclose(log);
			exit(0);
	}	}
	fclose(tga_in);

	
	//create image raster
	struct RasterImage *fRaster = readTGA(fname);/**< The input image data. */
	
	fprintf(log, "dimensions created a raster from the image data\n");
	//use argument flags to print requested output
	if (arguments[2] != 1) {
		if (arguments[0] == 1) printf("height: ");
		printf("%d", fRaster->numRows);
	} if (arguments[1] != 1) {
		if (arguments[2] != 1) printf(", ");
		if (arguments[0] == 1) printf("width: ");
		printf("%d", fRaster->numCols);
	} printf("\n");
	
	fprintf(log, "dimensions outputted: %dx%d\n", fRaster->numRows, fRaster->numCols);
	
	fprintf(log, "dimensions complete.\n");
	fclose(log);
	free(fRaster->raster2d);
	free(fRaster->raster);
	free(fRaster);
	return 0;
}


/**	
*	@brief A test that ensures the given input argument is valid
*	@param arg An integer representing the number of arguments
*	@return 0 if arg is -v
*	@return 1 if arg is -h
*	@return 2 if arg is -w
*	@return 3 if arg fails the test
*/ 
unsigned int test(char *arg) {
	if (strcmp(arg, "-v") == 0) return 0;
	else if (strcmp(arg, "-h") == 0) return 1;
	else if (strcmp(arg, "-w") == 0) return 2;
	return 3;
}	


