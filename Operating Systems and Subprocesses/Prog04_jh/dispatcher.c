
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//
#include "image_IO_TGA.h"

int main(int argc, char* argv[]) {
	unsigned int index = 1;
	char *tile[32];
	tile[4] = NULL;
	int i;
	while (index < argc) {
		unsigned int j = 0;
		if ((strcmp(argv[index],"./crop"))==0) {
			while (j<7) tile[j++] = argv[index++];
		} else if (strcmp(argv[index],"./rotate")==0) {
			while (j<4) tile[j++] = argv[index++];
		} else if ((strcmp(argv[index],"./split")==0)||(strcmp(argv[index],"./dimensions")==0)) {
			while (j<3) tile[j++] = argv[index++];
		} else {
			
			printf("unknown dispatch order = %s\n", argv[index++]);
			exit(0);
		}
		tile[j] = NULL;
		i = fork();
		if (i == 0) {
			execve(tile[0],tile,0);
			printf("attempt failed\n");
		}
		waitpid(i, &i, 0);
	}
}

