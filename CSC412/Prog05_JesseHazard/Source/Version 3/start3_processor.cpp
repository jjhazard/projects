
/**
	author : Jesse Hazard
	name : start3_processor
	Description : 
	This is the process function, it reads every list of files in its 
	assigned filename, organizes the lines, and writes it's portion of
	the final file in a single string back in the file. It then exits and
	never returns to main.
	
	Parameters:
	pNum - The index of the process in the overall set of processes
	
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

int compare(const void* a, const void* b);
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	int index = 0;
	string fname(argv[1]);
	fname = "P#"+fname+".txt";
	string line;
	ifstream read;
	read.open(fname);
	while (getline(read, line)) index++;
	read.close();

	char **chunk = new char*[index+1];
	chunk[index] = NULL;
	
	index = 0;
	int total = 0;
	read.open(fname);
	while (getline(read, line)) {

		ifstream read;
		read.open(line);
		getline(read, line);
		read.close();
		//get char array from line
		char *current = new char[line.length()+1];
		strcpy(current, line.c_str());
		strcpy(current, &current[2]);
		total += line.length()+1;

		//check line endings
		int i = 0;
		bool test = false;
		while (current[i] != '\0') {
			//make sure line end is newline character
			if ((current[i]==10) || (current[i]==12) || (current[i]==13)) {
				current[i] = '\n';
				test = true;
			} i++;
		}
		//if no line end, add one
		if (not test) strcat(current, "\n");
		//add to reference list
		chunk[index++] = current;
	}
	read.close();
			
	//sort strings
	qsort(chunk,index,sizeof(char*),compare);
	//first entry in full string
	char *complete = new char[total+1];
	int x = 1;
	while ((47<chunk[0][x])&&(chunk[0][x]<58)) x++;
	x++;
	strcpy(complete, &chunk[0][x]);
	delete[] chunk[0];
	chunk[0] = &complete[0];

	//add rest of strings
	for (int i=1; i<index; i++) {
		x = 1;
		while ((47<chunk[0][x])&&(chunk[0][x]<58)) x++;
		x++;
		strcat(complete, &chunk[i][x]);
		delete[] chunk[i];
	} delete[] chunk;
	//write to file
	ofstream newfile(fname);
	newfile << complete << endl;
	newfile.close();

	//clear memory and exit
	delete[] complete;
	exit(0);  
}


///////////////////////////////////////////////////////////////////////////////

int compare(const void* a, const void* b) {
	const char* x = *(char**) a;
	const char* y = *(char**) b;
	int c = 0;
	int d = 0;
	while ((47<x[c]) && (x[c]<58)) c++;
	while ((47<y[d]) && (y[d]<58)) d++;
	if (c > d) return 1;
	else if (c < d) return -1;
	c = 0;
	while (c < d) {
		if (x[c] > y[c]) return 1;
		else if (x[c] < y[c]) return -1;
		else c++;
	} return 0;
}
