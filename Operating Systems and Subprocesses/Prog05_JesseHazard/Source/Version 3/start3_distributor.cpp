
/**
	author : Jesse HAzard
	name: start3_distributor
	Description : 
	This is the distribute function, it reads the files it was assigned
	and determines which processor subprocesses should read which files.
	Once done, it writes those file names to a new file. It then exists
	and never returns to main.
	
	Parameters:
	*ipath - Pointer to a char array containing the input directory
	pNum - The process index
*/	
	
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
	//count process files
	int count = 0;
	struct dirent *entry;
	DIR *dir = opendir(argv[1]);
	if (dir == NULL) exit(0);
	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_name[0] != '.') && (entry->d_name[1] != '.')) {
			char output[100];
			strcpy(output,argv[1]);
			if (output[strlen(output)-1] != '/') strcat(output,"/");
			strcat(output,entry->d_name);
			ifstream read;
			read.open(output);
			output[0] = '\0';
			read >> output;
			read.close();
			if (atoi(output) == atoi(argv[2])) count+=1;
		}
	}
	closedir(dir);

	//create list for file names
	char **fList = new char*[count+1];
	fList[count] = NULL;
	count = 0;

	//get file names
	string opath(argv[2]);
	opath = "P#"+opath+".txt";
	ofstream newfile(opath);
	dir = opendir(argv[1]);
	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_name[0] != '.') && (entry->d_name[1] != '.')) {
			char* output = new char[128];
			strcpy(output,argv[1]);
			if (output[strlen(output)-1] != '/') strcat(output,"/");
			strcat(output,entry->d_name);
			ifstream read;
			read.open(output);
			char digit[8];
			read >> digit;
			string y = digit;
			read.close();
			if (atoi(digit) == atoi(argv[2])) {
				fList[count] = output;
				newfile << fList[count] << endl;
			} else delete[] output;
		}
	}
	closedir(dir);
	for (int i=0; i<count; i++) delete[] fList[i];
	delete[] fList;
	newfile.close();
	
	exit(0);
}

