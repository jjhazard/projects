/**
	@author Jesse Hazard
	@name version3
	This file contains the C++ code to read a lines of text from a directory
	containing only text files with a single line of code in each. When the
	program is finished executing, the new text file is built with the 
	desired name. It uses subprocesses and executables to speed the process 	up.
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

int main(int argc, char *argv[]) {

	if ((argc < 4) || (argc > 4)) exit(0);

	//arg1 is the number of child processes
	int pNum = 0;
	pNum = atoi(argv[1]);
	char* ipath = argv[2];
	char* opath = argv[3];

	//builds n lists of files//write file names
	int child[pNum] = { 0 };
	int j = 0;
	char buffer[128];
	strcpy(buffer, "./start3_distributor");
	char* args[4];
	args[0] = buffer;
	args[1] = ipath;
	args[3] = NULL;
	string hold;
	while (j < pNum) {
		child[j] = fork();
		if (child[j] == 0) {
			hold = to_string(j);
			strcpy(&buffer[22], hold.c_str());
			args[2] = &buffer[22];
			execve(args[0],args,0);
		}
		j++;
	}
	int status;
	for (int i=0; i<pNum; i++) {
		waitpid(child[i], &status, 0);
		child[i] = 0;
	}
	//process the data from each set of files
	j = 0;
	strcpy(buffer, "./start3_processor");
	args[2] = NULL;
	while (j < pNum) {
		child[j] = fork();
		if (child[j] == 0) {
			hold = to_string(j);
			strcpy(&buffer[20], hold.c_str());
			args[1] = &buffer[20];
			execve(args[0],args,0);
		}
		j++;
	}
	for (int i=0; i<pNum; i++) {
		waitpid(child[i], &status, 0);
	}
	
	//read from the process files and write to the new file
	ifstream read;
	ofstream newfile(opath);
	string line;
	for (int i=0; i<pNum; i++) {
		read.open("P#"+to_string(i)+".txt");
		while (getline(read, line)) newfile << line << endl;
		read.close();
		line = "P#"+to_string(i)+".txt";
		remove(line.c_str());
	}
}
