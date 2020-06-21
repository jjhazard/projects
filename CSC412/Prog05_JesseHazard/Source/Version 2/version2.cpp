/**
	@author Jesse Hazard
	@name version2
	This file contains the C++ code to read a lines of text from a directory
	containing only text files with a single line of code in each. When the
	program is finished executing, the new text file is built with the 
	desired name. It uses subprocesses to speed the process up.
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

void distribute(char* ipath, int pNum);
void process(int pNum);
int compare(const void* a, const void* b);
int compareChar(char a, char b);
/**
	Function: main
	Description : 
	This is the main function, it creates subprocesses that call the
	distributor function n times to create n lists of n lists of files
	and write them to a new set of organized filenames then exit before
	returning. Once the subprocesses exit, the main function repeats this
	process on the processor function; calling it n times to
	read the file, create the program strings, and write them back to the
	file. Once that is complete, is reads those strings and writes a
	directly to the a new file with opath.
	
	Parameters:
	*fileName - Pointer to a string that contains the name of the file
	*pNum - The number of processes
	*ipath - The input directory containing the text files
	opath - The output file name
*/	


int main(int argc, char *argv[]) {

	if ((argc < 4) || (argc > 4)) exit(0);

	//arg1 is the number of child processes
	int pNum = 0;
	pNum = atoi(argv[1]);
	char* ipath = argv[2];
	string opath = argv[3];

	//builds n lists of files//write file names
	int child[pNum] = { 0 };
	int j = 0;
	while (j < pNum) {
		child[j] = fork();
		if (child[j] == 0) distribute(ipath, j);
		j++;
	}
	int status;
	for (int i=0; i<pNum; i++) {
		waitpid(child[i], &status, 0);
		child[i] = 0;
	}

	//process the data from each set of files
	j = 0;
	while (j < pNum) {
		child[j] = fork();
		if (child[j] == 0) process(j);;
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


///////////////////////////////////////////////////////////////////////////////

/**
	Function: distribute
	Description : 
	This is the distribute function, it reads the files it was assigned
	and determines which processor subprocesses should read which files.
	Once done, it writes those file names to a new file. It then exists
	and never returns to main.
	
	Parameters:
	*ipath - Pointer to a char array containing the input directory
	pNum - The process index
*/	
	

void distribute(char* ipath, int pNum) {

	//count process files
	int count = 0;
	struct dirent *entry;
	DIR *dir = opendir(ipath);
	if (dir == NULL) exit(0);
	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_name[0] != '.') && (entry->d_name[1] != '.')) {
			char output[100];
			strcpy(output,ipath);
			if (output[strlen(output)-1] != '/') strcat(output,"/");
			strcat(output,entry->d_name);
			ifstream read;
			read.open(output);
			output[0] = '\0';
			read >> output;
			read.close();
			if (atoi(output) == pNum) count+=1;
		}
	}
	closedir(dir);

	//create list for file names
	char **fList = new char*[count+1];
	fList[count] = NULL;
	count = 0;

	//get file names
	string opath("P#"+to_string(pNum)+".txt");
	ofstream newfile(opath);
	dir = opendir(ipath);
	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_name[0] != '.') && (entry->d_name[1] != '.')) {
			char* output = new char[128];
			strcpy(output,ipath);
			if (output[strlen(output)-1] != '/') strcat(output,"/");
			strcat(output,entry->d_name);
			ifstream read;
			read.open(output);
			char digit[8];
			read >> digit;
			string y = digit;
			read.close();
			if (atoi(digit) == pNum) {
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


///////////////////////////////////////////////////////////////////////////////

/**
	Function: process
	Description : 
	This is the process function, it reads every list of files in its 
	assigned filename, organizes the lines, and writes it's portion of
	the final file in a single string back in the file. It then exits and
	never returns to main.
	
	Parameters:
	pNum - The index of the process in the overall set of processes
	
*/	

void process(int pNum) {

	int index = 0;
	string fname("P#"+to_string(pNum)+".txt");
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
/**
	Function: compare
	Description : 
	This function is a qsort function that compares two file strings by the
	first number in the file string. The file strings always contain a
	number first followed by a space, so it checks the characters as being
	digits and compares the length. Then, if the length is equal, it
	compares the digit values. THe digits are arranged in ascending order.
	
	Parameters:
	*a - Pointer to a string that contains the first file line
	*b - Pointer to a string that contains the second file line

	Return Value : -1 if a > b, 1 if a < b, and 0 otherwise

*/

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
