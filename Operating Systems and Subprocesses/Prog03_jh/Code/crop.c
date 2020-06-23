/**
 *	@file crop.c
 *
 *	@brief This is a program to crop an image.
 *	It takes as arguments an input file,
 *	an output directory, x and y coordinates 
 *	that point to the bottom left corner of
 *	the cropped image, and x and y dimensions
 *	for the cropped image.
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

/**	
*	
*	@brief Main function
*	This function takes as arguments an input file path,
*	an output directory, x and y coordinates for the new image,
*	and x and y dimensions for the new image. It verifies
*	the arguments, copies the bits at the desired coordinates to the bottom *	left of the image, and adjusts the column and row information to
*	reflect eh dimensions of the new image.
*	@param argc An integer representing the number of arguments
*	@param arv[] The array of arguments in char* form 
*	@return 0 if successfully complete
*
*/
int main(int argc, char** argv) {

	FILE *log = fopen("crop_log.txt","a");/**<crop's log file */
	fprintf(log, "crop called.\n");

	//Validate argument count
	if (argc > 7){
		printf("crop recieved too many arguments.\n");
		fprintf(log, "crop closed because it recieved too few arguments.\n");
		fclose(log);
		exit(0);
	} else if (argc < 7) {
		printf("crop recieved too few arguments.\n");
		fprintf(log, "crop closed because it recieved too few arguments.\n");
		fclose(log);
		exit(0);
	}

	//validate input numbers
	for (unsigned int i=3;i<7;i++) {
		unsigned int j = 0;
		char *c = argv[i];
		while (c[j] != '\0') {
			if ((*c < 48) || (*c > 57)) {
				printf("%s is not a strictly positive integer\n", c);
				fprintf(log, "crop closed because the coordinates and dimensions were not all positive integers.\n");
				fclose(log);
				exit(0);
			} j++;
		}
	}
	
	//validate paths
	char	*finput = argv[1];/**< The input file path. */
	char	*foutput = argv[2];/**< The output directory. */

	if (access(finput, F_OK) == -1) {
		printf("%s is not a valid file path\n", argv[1]);
		fprintf(log, "crop closed because the file path was invalid.\n");
		fclose(log);
		exit(0);
	} else if (strcmp(&finput[strlen(finput)-4], ".tga") != 0) {
		printf("%s is not a .tga file\n", finput);
		fprintf(log, "crop closed because the file was not a tga image.\n");
		fclose(log);
		exit(0);
	} else {

		//create struct for validation
		struct stat test_stat;/**< A struct to verify the inputs. */
		stat(finput, &test_stat);
		if (test_stat.st_size <= 1) {
			printf("%s is empty\n", finput);
			fprintf(log, "crop closed because the tga file had no data.\n");
			fclose(log);
			exit(0);
		}
		stat(foutput, &test_stat);
		if (S_ISDIR(test_stat.st_mode) == 0) {
			printf("%s is not a directory\n", foutput);
			fprintf(log, "crop closed because the output directory was invalid.\n");
			fclose(log);
			exit(0);
	}	}
	
	//validate file content
	FILE	*tga_in;/**< The input file. */
	tga_in = fopen(finput, "rb" );
	if (tga_in == NULL) {
		printf("Cannot open image file\n");
		fprintf(log, "crop closed because the tga file was empty.\n");
		fclose(log);
		exit(1);
	}
	fclose(tga_in);

	//create image raster
	struct RasterImage *fRaster = readTGA(finput); /**<The complete image 
							*  data from the file
							*/	 
	fprintf(log, "read image data from file %s.\n", finput);

	//validate parameter dimensions
	unsigned int coord[2];/**< The input x and y coordinates. */
	coord[0] = atoi(argv[3]);
	coord[1] = atoi(argv[4]);
	unsigned int dimen[2];/**< The input x and y dimensions. */
	dimen[0] = atoi(argv[5]);
	dimen[1] = atoi(argv[6]);
	
	if (((dimen[0] + coord[0])>fRaster->numRows)||((dimen[1] + coord[1])>fRaster->numCols)) {
		printf("crop dimensions do not fall within original image\n");
		free(fRaster->raster);
		free(fRaster);
		fprintf(log, "crop closed because the requested cropped image was outside the bounds of the original image.\n");
		fclose(log);
		exit(0);
	}
	fprintf(log, "requested coordinates: (%d,%d)\n", coord[0], coord[1]);
	fprintf(log, "requested dimensions: %dx%d\n", dimen[0], dimen[1]);
	
	//crop image
	for (unsigned int i=0; i<dimen[0]; i++) {
		for (unsigned int j=0; j<dimen[1]; j++) {
			for (unsigned int k=0; k<4; k++) {
			
			fRaster->raster2d[i][4*j + k] = fRaster->raster2d[i+coord[0]][4*(j+coord[1]) + k];

			}
		}
	}
	fRaster->numRows = dimen[0];
	fRaster->numCols = dimen[1];
	fprintf(log, "cropped raster created.\n");

	//create new file path
	char newPath[150];/**< The file path for the new image. */
	char pathHold[150];
	unsigned int index = (strlen(finput)-1);
	while (finput[index] != '/') index--;
	finput = &finput[index+1];
	finput[strlen(finput)-4] = '\0';
	strcpy(newPath,foutput);
	if (foutput[strlen(foutput)-1] != '/') strcat(newPath,"/"); 
	strcat(newPath,finput);
	strcat(newPath," [cropped 1].tga");

	
	//if [cropped 1] does not exist
	if (access(newPath, F_OK) == -1) {
		strcpy(&newPath[strlen(newPath)-7], &newPath[strlen(newPath)-5]);

		//if [cropped] does not exist
		if (access(newPath, F_OK) == -1) {
			
			//try to write [cropped]
			if (writeTGA(newPath, fRaster->raster2d, fRaster->numRows, fRaster->numCols, 3) != 0) {
				printf("failed to write cropped image to %s\n", newPath);
				fprintf(log, "crop failed to write %s.\n", newPath);
				} else fprintf(log, "crop created file %s.\n", newPath);

		//if [cropped] does exist
		} else {

			//rename to [cropped 1]
			strcpy(pathHold, newPath);
			newPath[strlen(newPath)-5] = '\0';
			strcat(newPath, " 1].tga");
			rename(pathHold, newPath);

			//try to write [cropped 2]
			newPath[strlen(newPath)-6] = '\0';
			strcat(newPath, "2].tga");
			if (writeTGA(newPath, fRaster->raster2d, fRaster->numRows, fRaster->numCols, 3) != 0) {
				printf("failed to write cropped image to %s\n", newPath);
				fprintf(log, "crop failed to write %s.\n", newPath);
			} else fprintf(log, "crop created file %s.\n", newPath);
	}//if cropped 1 does exist
	} else {

		//increment until filename available
		index = 2;
		while (access(newPath, F_OK) != -1) {
			char count[10];
			sprintf(count, "%d", index);
			if (index < 11) newPath[strlen(newPath)-6] = '\0';
			else newPath[strlen(newPath)-7] = '\0';
			strcat(newPath, count);
			strcat(newPath, "].tga");
			index++;
		} 
		if (writeTGA(newPath, fRaster->raster2d, fRaster->numRows, fRaster->numCols, 3) != 0) {
			printf("failed to write cropped image to %s\n", newPath);
			fprintf(log, "crop failed to write %s.\n", newPath);
		} else fprintf(log, "crop created file %s.\n", newPath);
	}

	//free allocation and close log file
	fprintf(log, "crop complete.\n");
	fclose(log);
	free(fRaster->raster2d);
	free(fRaster->raster);
	free(fRaster);
	return 0;
}
