/**
	@author Jesse Hazard
	@name version1
	This file contains the C++ code to read a lines of text from a directory
	containing only text files with a single line of code in each. When the
	program is finished executing, the new text file is built with the 
	desired name. 
*/
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <cstdlib>


using namespace std;

char*** distribute(char* ipath, int pNum, int current, int count);
char* process(char**** file, int pInd);
int compare(const void* a, const void* b);
int compareChar(char a, char b);

/**
	Function: main
	Description : 
	This is the main function, it calls the distributor function n times to 
	create n lists of n lists of files. It then calls the processor
	function n times to read the files and create the program strings.
	Once that is complete, is writes those strings directly to the a new
	file with the desired name.
	
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
 
	//count files
	int count = 0;
	struct dirent *entry;
	DIR *dir = opendir(ipath);
	if (dir == NULL) exit(0);
	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_name[0] != '.') && (entry->d_name[1] != '.')) {
			count+=1;
		}
	}
	closedir(dir);
	count = int(count/pNum)+1;

	//builds n lists of files
	char**** fLists = new char***[pNum+1];
	for (int i=0; i<pNum; i++) fLists[i] = distribute(ipath, pNum, i, count);
	fLists[pNum] = NULL;
	//processes the data from each
	char** fNew = new char*[pNum+1];
	for (int i=0; i<pNum; i++) fNew[i] = process(fLists, i);

	
	//write the new filec++ write 
	ofstream newfile(opath);
	if (newfile) {
		for (int i=0; i<pNum; i++) {
			int j =0;
			newfile << fNew[i] << endl;
			while (fLists[i][j] != NULL) {
				int k = 0;
				while (fLists[i][j][k]!=NULL) delete[] fLists[i][j][k++];
				delete[] fLists[i][j++];
			} delete[] fLists[i];
			delete[] fNew[i];
		}
		newfile.close();
	} else {
		for (int i=0; i<pNum; i++) {
			int j =0;
			while (fLists[i][j] != NULL) {
				int k = 0;
				while (fLists[i][j][k]!=NULL) delete[] fLists[i][j][k++];
				delete[] fLists[i][j++];
			} delete[] fLists[i];
			delete[] fNew[i];
		}
	}
	delete[] fNew;
	delete[] fLists;	
}


///////////////////////////////////////////////////////////////////////////////

/**
	Function: distribute
	Description : 
	This is the distribute function, it reads the files it was assigned
	and determines which processor subprocesses should read which files.
	Once done, it writes those file names to a new file.
	
	Parameters:
	*ipath - Pointer to a char array containing the input directory
	pNum - The number of processes
	current - The index of the process in the overall set of processes
	count - the number of files the dispatcher should process.
	
	
*/	

char*** distribute(char* ipath, int pNum, int current, int count) {
	//count process files
	int index = 0;
	struct dirent *entry;
	DIR *dir = opendir(ipath);
	if (dir == NULL) exit(0);
	//create lists for file names
	char ***fList = new char**[pNum+1];
	for (int i=0; i<pNum; i++) fList[i] = new char*[count+1];
	fList[pNum] = NULL;
	int counts[pNum] = { 0 };
	//get file names
	dir = opendir(ipath);
	while (((entry = readdir(dir)) != NULL) && (index < current*count)) index++;	
	count = count + index;
	while (((entry = readdir(dir)) != NULL) && (index <= count)) {
		if ((entry->d_name[0] != '.') && ((entry->d_name[1] != '\0') || (entry->d_name[1] != '\0'))) {
			char* output = new char[128];
			strcpy(&output[0],ipath);
			if (output[strlen(output)-1] != '/') strcat(output,"/");
			strcat(output,entry->d_name);
			ifstream read;
			read.open(output);
			char digit[8];
			read >> digit;
			read.close();
			int x = atoi(digit);
			fList[x][counts[x]++] = output;
		}
		index++;
	}
	closedir(dir);
	for (int i=0; i<pNum; i++) fList[i][counts[i]] = NULL;
	int i = -1;
	int j = 0;
	return fList;
}


///////////////////////////////////////////////////////////////////////////////

/**
	Function: process
	Description : 
	This is the process function, it reads every list of files in the input
	file list and access its associated files at int index [x][pInd][y].
	Once that is complete, it trims the index numbers off the lines and
	converts them to a single string. It returns the full file string for
	its process index.
	
	Parameters:
	****fileName - Pointer to a list of lists of filelists.
	*pInd - The index of the process in the overall set of processes
	
	Return Value: Complete file string for process index	
*/	

char* process(char**** file, int pInd) {

	//create array of strings
	int index = 0;
	int count = 0;
	int totallength = 0;
	while (file[index] != NULL) {
		while (file[index][pInd][count++] != NULL) totallength++;
		index++;
		count = 0;
	}
	char **chunk = new char*[totallength+1];
	chunk[totallength] = NULL;
	//read files
	count = 0;
	index = 0;
	int cindex = 0;
	int total = 0;
	int lineNum = 0;
	while (file[index] != NULL) {
		while (file[index][pInd][count] != NULL) {
			//read file line
			string line;
			ifstream read;
			read.open(file[index][pInd][count++]);
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
			chunk[cindex++] = current;
		}
		index++;
		count = 0;
	}
	//sort strings
	qsort(chunk,cindex,sizeof(char*),compare);
	//first entry in full string
	char *complete = new char[total+1];
	complete[0] = '\0';
	int x = 1;
	while ((47<chunk[0][x])&&(chunk[0][x]<58)) x++;
	x++;
	strcpy(complete, &chunk[0][x]);
	delete[] chunk[0];
	chunk[0] = &complete[0];
	//add rest of strings
	
	for (int i=1; i<cindex; i++) {
		x = 1;
		while ((47<chunk[0][x])&&(chunk[0][x]<58)) x++;
		x++;
		strcat(complete, &chunk[i][x]);
		delete[] chunk[i];
	} delete[] chunk;
	return complete;   
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
