//
//  main.c
//  Final Project CSC412
//
//  Created by Jean-Yves Hervé on 2019-12-12
//	This is public domain code.  By all means appropriate it and change is to your
//	heart's content.
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
//
#include "gl_frontEnd.h"
#include "robot.h"

using namespace std;

//==================================================================================
//	Function prototypes
//==================================================================================
void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);
void cleanupGridAndLists(void);

//==================================================================================
//	Application-level global variables
//==================================================================================

//	Don't touch
extern int	GRID_PANE, STATE_PANE;
extern int 	GRID_PANE_WIDTH, GRID_PANE_HEIGHT;
extern int	gMainWindow, gSubwindow[2];

//	Don't rename any of these variables
//-------------------------------------
//	The state grid and its dimensions (arguments to the program)
int** grid;
int numRows = -1;	//	height of the grid
int numCols = -1;	//	width
int numBoxes = -1;	//	also the number of robots
int numDoors = -1;	//	The number of doors.

int numLiveThreads = 0;		//	the number of live robot threads

//	robot sleep time between moves (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int robotSleepTime = 100000;

//	An array of C-string where you can store things you want displayed
//	in the state pane to display (for debugging purposes?)
//	Dont change the dimensions as this may break the front end
const int MAX_NUM_MESSAGES = 8;
const int MAX_LENGTH_MESSAGE = 32;
char** message;
time_t startTime;
int **robotLoc;
int **boxLoc;
int **doorLoc;
int *doorAssign;
gridWorld* scene;
FILE *fp;
//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================


void displayGridPane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, GRID_PANE_HEIGHT, 0);
	glScalef(1.f, -1.f, 1.f);
	
	//-----------------------------
	//	CHANGE THIS
	//-----------------------------
	//	Here I hard-code myself some data for robots and doors.  Obviously this code
	//	this code must go away.  I just want to show you how to display the information
	//	about a robot-box pair or a door.
	//	Important here:  I don't think of the locations (robot/box/door) as x and y, but
	//	as row and column.  So, the first index is a row (y) coordinate, and the second
	//	index is a column (x) coordinate.

    for (int i=0; i<numDoors; i++)
    {
        drawDoor(scene->doorList[i]->doorID, scene->doorList[i]->doorY, scene->doorList[i]->doorX);
    }

    //create robots and boxes

    for (int i=0; i<numBoxes; i++)
    {
        drawRobotAndBox(i, scene->robotList[i]->robY, scene->robotList[i]->robX,
                        scene->robotList[i]->boxY, scene->robotList[i]->boxX, scene->robotList[i]->assignedDoor->doorID);
    }
	//	This call does nothing important. It only draws lines
	//	There is nothing to synchronize here
	drawGrid();

	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

void displayStatePane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	Here I hard-code a few messages that I want to see displayed
	//	in my state pane.  The number of live robot threads will
	//	always get displayed.  No need to pass a message about it.
	time_t currentTime = time(NULL);
	double deltaT = difftime(currentTime, startTime);

	int numMessages = 3;
	sprintf(message[0], "We have %d doors", numDoors);
	sprintf(message[1], "I like cheese");
	sprintf(message[2], "Run time is %4.0f", deltaT);

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawState(numMessages, message);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupRobots(void)
{
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * robotSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		robotSleepTime = newSleepTime;
	}
}

void slowdownRobots(void)
{
	//	increase sleep time by 20%
	robotSleepTime = (12 * robotSleepTime) / 10;
}




//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function besides
//	the initialization of numRows, numCos, numDoors, numBoxes.
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
	//	We know that the arguments  of the program  are going
	//	to be the width (number of columns) and height (number of rows) of the
	//	grid, the number of boxes (and robots), and the number of doors.
	//	You are going to have to extract these.  For the time being,
	//	I hard code-some values
	if (argc != 5) {
		std::cout<<"Invalid arg number"<<std::endl;
		exit(0);
	}
	numRows = atoi(argv[1]);
	numCols = atoi(argv[2]);
	numDoors = atoi(argv[3]);
	numBoxes = atoi(argv[4]);
	if ((numDoors < 1) or (numDoors > 3)) {
		std::cout<<"Invalid door number"<<std::endl;
		exit(0);
	}
	if ((numRows < 3) or (numCols < 3)) {
		std::cout<<"Invalid dimensions (min = 3x3)"<<std::endl;
		exit(0);
	}


	fp = fopen("robotSimulOut.txt", "w+");
	fprintf(fp, "Rows %d, Columns %d, Boxes %d, Doors %d\n\n", numRows, numCols, numBoxes, numDoors);
	
	//	Even though we extracted the relevant information from the argument
	//	list, I still need to pass argc and argv to the front-end init
	//	function because that function passes them to glutInit, the required call
	//	to the initialization of the glut library.
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
	
	//	Now we can do application-level initialization
	initializeApplication();
	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	cleanupGridAndLists();

	fclose(fp);
    for (int i=0; i<numDoors; i++) free(doorLoc[i]);
    free(doorLoc);
    free(doorAssign); //	door id assigned to each robot-box pair

    for (int i=0; i<numBoxes; i++) {
        free(robotLoc[i]);
        free(boxLoc[i]);
    }
    free(robotLoc);
    free(boxLoc);
    //create doors
	//	This will probably never be executed (the exit point will be in one of the
	//	call back functions).
    return 0;
}

//---------------------------------------------------------------------
//	Free allocated resource before leaving (not absolutely needed, but
//	just nicer.  Also, if you crash there, you know something is wrong
//	in your code.
//---------------------------------------------------------------------
void cleanupGridAndLists(void)
{
	for (int i=0; i< numRows; i++)
		free(grid[i]);
	free(grid);
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		free(message[k]);
	free(message);
}


//==================================================================================
//
//	This is a part that you have to edit and add to.
//
//==================================================================================


void initializeApplication(void)
{
	//	Allocate the grid
	grid = (int**) malloc(numRows * sizeof(int*));
	for (int i=0; i<numRows; i++)
		grid[i] = (int*) malloc(numCols * sizeof(int));
	message = (char**) malloc(MAX_NUM_MESSAGES*sizeof(char*));
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		message[k] = (char*)malloc((MAX_LENGTH_MESSAGE+1)*sizeof(char));
	startTime = time(NULL);
	srand((unsigned int) startTime);
	scene = new gridWorld();
	scene->grid = grid;
	scene->numCols = numCols; //height
	scene->numRows = numRows; //width
	scene->fp = fp;
	pthread_mutex_init(&scene->filelock, NULL);

	robotLoc = (int**)malloc(numBoxes*sizeof(int*));
	boxLoc = (int**)malloc(numBoxes*sizeof(int*));
	doorLoc= (int**)malloc(numDoors*sizeof(int*));
	doorAssign = (int*)malloc(numDoors*sizeof(int)); //	door id assigned to each robot-box pair

	//create doors
	for (int i=0; i<numDoors; i++) {
		doorLoc[i] = (int*)malloc(2*sizeof(int));
		doorLoc[i][0] = rand() % scene->numRows;
		doorLoc[i][1] = rand() % scene->numCols;

		int j = 0;
		while (j < i) {
			if ((doorLoc[i][0] == scene->doorList[j]->doorY) &&
			(doorLoc[i][1] == scene->doorList[j]->doorX)) {
				doorLoc[i][1] = rand() % scene->numCols;
				j = 0;
			}
			j++;
		}

		fprintf(fp,"Door %d: Row %d, Column %d\n",i+1,doorLoc[i][0],doorLoc[i][1]);
		if (i == numDoors-1) fprintf(fp,"\n");

		doorAssign[i] = rand() % numDoors;
		door* theDoor = new door();
		theDoor->doorID = i;
		theDoor->doorX = doorLoc[i][1]; //1 is column, 0 is row
		theDoor->doorY = doorLoc[i][0];
		scene->doorList.push_back(theDoor);
	}

	//create robots and boxes

	for (int i=0; i<numBoxes; i++) {
		boxLoc[i] = (int*)malloc(2*sizeof(int));
		boxLoc[i][0] = 1 + (rand() % (scene->numRows-2));
		boxLoc[i][1] = 1 + (rand() % (scene->numCols-2));
		int test = 1;

		//Test for repeat coordinates
		while (test > -1) {
			test = 1;
			int j = 0;

			//test robot against door coordinates
			while (j < numDoors) {
				if ((boxLoc[i][0]==scene->doorList[j]->doorY) &&
				(boxLoc[i][1] == scene->doorList[j]->doorX)) {
				boxLoc[i][1] = 1+(rand() % (scene->numCols-2));
					j = 0;
				}
				j++;
			}
			j = 0;

			//test robot against robot coordinates
			while ((j < i) && (test > 0)) {
				if ((boxLoc[i][0] == boxLoc[j][0]) &&
				(boxLoc[i][1] == boxLoc[j][1])) {
				boxLoc[i][1] = 1+(rand() % (scene->numCols-2));
					test = 0;
				}
				j++;
			}

			//if no duplicates, test successful
			if (j == i) test = -1;

		}

		fprintf(fp,"Box %d: Row %d, Column %d\n",i+1,boxLoc[i][0],boxLoc[i][1]);
		if (i == numBoxes-1) fprintf(fp,"\n");

	}

	//create box coordinates
	for (int i=0; i<numBoxes; i++) {
		robotLoc[i] = (int *) malloc(2 * sizeof(int));
		robotLoc[i][0] = rand() % scene->numRows;
		robotLoc[i][1] = rand() % scene->numCols;
		int test = 1;

		//test for repeat coordinates
		while (test > -1) {
			test = 1;
			int j = 0;

			//test against door coordinates
			while (j < numDoors) {
				if ((robotLoc[i][0]==scene->doorList[j]->doorY) 
				&&(robotLoc[i][1]==scene->doorList[j]->doorX)) {
				robotLoc[i][1] = 1+(rand()%(scene->numCols-2));
					j = 0;
				}
				j++;
			}
			j = 0;

			//test against robot coordinates
			while ((j < numBoxes) && (test > 0)) {
				if ((robotLoc[i][0] == boxLoc[j][0]) &&
				(robotLoc[i][1] == boxLoc[j][1])) {
				robotLoc[i][1] = 1+(rand()%(scene->numCols-2));
					test = 0;
				}
				j++;
			}
			j = 0;

			//test against box coordinates
			while ((j < i) && (test > 0)) {
				if ((robotLoc[i][0] == robotLoc[j][0]) &&
				(robotLoc[i][1] == robotLoc[j][1])) {
				robotLoc[i][1] = 1+(rand()%(scene->numCols-2));
					test = 0;
				}
				j++;
			}

		//if no duplicates, test successful
		if (j == i) test = -1;
		}

	

		//initialize robot and box
		robotAndBox* roboBox = new robotAndBox();
		roboBox->robX = robotLoc[i][1];
		roboBox->robY = robotLoc[i][0];
		roboBox->boxX = boxLoc[i][1];
		roboBox->boxY = boxLoc[i][0];
		roboBox->assignedDoor = scene->doorList[i%numDoors];

		//store robot and box into robotlist
		scene->robotList.push_back(roboBox);


		fprintf(fp,"Robot %d: Row %d, Column %d, Door %d\n",i+1,robotLoc[i][0],robotLoc[i][1],roboBox->assignedDoor->doorID);
		if (i == numBoxes-1) fprintf(fp,"\n");
	}
	//initalize threads
	int errCode = 0;
	for (int i=0; i<numBoxes; i++) {
		struct robotThread* rob = new robotThread;
		rob->threadIndex = i;
		rob->grid = scene;
		rob->robox = scene->robotList[i];
		rob->isActive = true;
		errCode = pthread_create(&rob->id, NULL, robotFunc, (void*)rob);
		if (errCode != 0) exit(0);
	}
}


