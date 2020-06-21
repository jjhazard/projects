/**
 *	@file split.c
 *
 *	@brief This program splits an image into a blue, red, and green image.
 *	split takes as arguments an input file and an output directory.
 *	It reads image data and prints 3 separate images with altered color  
 *	channels.
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
*	This function takes as arguments an input file path and an
*	an output directory, x and y coordinates for the new image,
*	and x and y dimensions for the new image. It verifies
*	the arguments then it writes three image files, killing two
*	different color channels with a blank char each time.
*	@param argc An integer representing the number of arguments
*	@param arv[] The array of arguments in char* form 
*	@return 0 if successfully complete
*
*/
int main(int argc, char** argv) {

	FILE *log = fopen("split_log.txt","a");/**< split's log file. */
	fprintf(log, "split called.\n");

	//Validate argument count
	if (argc > 3){
		printf("crop recieved too many arguments.\n");
		fprintf(log, "split closed due to too many arguments\n");
		fclose(log);
		exit(0);
	} else if (argc < 3) {
		printf("crop recieved too few arguments.\n");
		fprintf(log, "split closed due to too few arguments\n");
		fclose(log);
		exit(0);
	}
	
	//validate paths
	char	*finput = argv[1];/**< The input file path. */
	char	*foutput = argv[2];/**< The output direcory. */

	if (access(finput, F_OK) == -1) {
		printf("%s is not a valid file path\n", argv[1]);
		fprintf(log, "split closed because %s is not a file path.\n", finput);
		fclose(log);
		exit(0);
	} else if (strcmp(&finput[strlen(finput)-4], ".tga") != 0) {
		printf("%s is not a .tga file\n", finput);
		fprintf(log, "split closed because %s is not a tga file.\n", finput);
		fclose(log);
		exit(0);
	} else {

		//create struct for validation
		struct stat test_stat;/**< A struct to verify the input paths. */
		stat(finput, &test_stat);
		if (test_stat.st_size <= 1) {
			printf("%s is empty\n", finput);
			fprintf(log, "split closed because %s is empty.\n", finput);
			fclose(log);
			exit(0);
		}
		stat(foutput, &test_stat);
		if (S_ISDIR(test_stat.st_mode) == 0) {
			printf("%s is not a directory\n", foutput);
			fprintf(log, "split closed because %s is not a directory.\n", foutput);
			fclose(log);
			exit(0);
		}
	}
	
	//validate file content
	FILE	*tga_in;/**< The open input file. */
	tga_in = fopen(finput, "rb" );
	if (tga_in == NULL) {
		printf("Cannot open image file\n");
		fprintf(log, "split closed because %s is empty.\n", finput);
		fclose(log);
		exit(1);
	}
	fclose(tga_in);

	//create image raster
	struct RasterImage *orig = readTGA(finput);/**< The raster read from the image file. */
	fprintf(log, "split created raster from image %s\n", finput);
	
	//get file name
	char newPath[150];/**< The new file path. */
	unsigned int index = (strlen(finput)-1);
	while (finput[index] != '/') index--;
	finput = &finput[index+1];
	finput[strlen(finput)-4] = '\0';
	
	for (int color=0; color<3; color++) {
		//create new file path
		strcpy(newPath,foutput);
		if (foutput[strlen(foutput)-1] != '/') strcat(newPath,"/"); 
		strcat(newPath,finput);
		if (color == 0) strcat(newPath,"_r [1].tga");
		else if (color == 1) strcat(newPath,"_g [1].tga");
		else strcat(newPath,"_b [1].tga");

		//if [1] does not exist
		int hold = access(newPath, F_OK);
		if (hold == -1) {
			strcpy(&newPath[strlen(newPath)-8],&newPath[strlen(newPath)-4]);

			//if base case does not exist
			hold = access(newPath, F_OK);
			if (hold == -1) {
				
				//try to write
				if (writeTGA(newPath, orig->raster2d, orig->numRows, orig->numCols, color) != 0) {
					printf("failed to write split image to %s\n", newPath);
					fprintf(log, "split failed to write %s.\n", newPath);
				} else fprintf(log, "split wrote file %s.\n", newPath);

			//if base case does exist
			} else {
				char pathHold[150];
				//rename to [cropped 1]
				strcpy(pathHold, newPath);
				newPath[strlen(newPath)-4] = '\0';
				strcat(newPath, " [1].tga");
				rename(pathHold, newPath);

				//try to write [cropped 2]
				newPath[strlen(newPath)-8] = '\0';
				strcat(newPath, " [2].tga");
				if (writeTGA(newPath, orig->raster2d, orig->numRows, orig->numCols, color) != 0) {
					printf("failed to write split image to %s\n", newPath);
					fprintf(log, "split failed to write %s.\n", newPath);
				} else fprintf(log, "split wrote file %s.\n", newPath);
			}

		//if cropped 1 does exist
		} else {

			//increment until filename available
			index = 2;
			hold = 0;
			while (hold != -1) {
				char count[10];
				sprintf(count, "%d", index);
				if (index < 11) newPath[strlen(newPath)-6] = '\0';
				else newPath[strlen(newPath)-7] = '\0';
				strcat(newPath, count);
				strcat(newPath, "].tga");
				index++;
				hold = access(newPath, F_OK);
			} 
			if (writeTGA(newPath, orig->raster2d, orig->numRows, orig->numCols, color) != 0) {
				printf("failed to write split image to %s\n", newPath);
				fprintf(log, "split failed to write %s.\n", newPath);
			} else fprintf(log, "split wrote file %s.\n", newPath);
		}
	}

	fprintf(log, "split complete.\n");
	fclose(log);
	free(orig->raster2d);
	free(orig->raster);
	free(orig);
	return 0;
}
