//
//  main.cpp
//  GL threads
//
//  Created by Jean-Yves Hervé on 2017-04-24, revised 2019-11-19
//  Revised by Jesse Hazard on 2019-12-2
//

 /*-------------------------------------------------------------------------+
 |	A graphic front end for a grid+state simulation.						|
 |																			|
 |	This application simply creates a glut window with a pane to display	|
 |	a colored grid and the other to display some state information.			|
 |	Sets up callback functions to handle menu, mouse and keyboard events.	|
 |	Normally, you shouldn't have to touch anything in this code, unless		|
 |	you want to change some of the things displayed, add menus, etc.		|
 |	Only mess with this after everything else works and making a backup		|
 |	copy of your project.  OpenGL & glut are tricky and it's really easy	|
 |	to break everything with a single line of code.							|
 |																			|
 |	Current GUI:															|
 |		- 'ESC' --> exit the application									|
 |		- 'r' --> add red ink												|
 |		- 'g' --> add green ink												|
 |		- 'b' --> add blue ink												|
 +-------------------------------------------------------------------------*/
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
//
#include "gl_frontEnd.h"

using namespace std;

typedef struct ThreadInfo {
	pthread_t threadID;
	unsigned int index;
} ThreadInfo;
ThreadInfo* threads;
pthread_mutex_t** locks;
pthread_mutex_t* colorLocks;
void* traveller(void* param);
//==================================================================================
//	Function prototypes
//==================================================================================
void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);


//==================================================================================
//	Application-level global variables
//==================================================================================

//	Don't touch
extern int	GRID_PANE, STATE_PANE;
extern int	gMainWindow, gSubwindow[2];

//	The state grid and its dimensions
int** grid;
const int NUM_ROWS = 20, NUM_COLS = 20;

//	the number of live threads (that haven't terminated yet)
int MAX_NUM_TRAVELER_THREADS = 10;
int numLiveThreads = 0;

//	the ink levels
int MAX_LEVEL = 50;
int MAX_ADD_INK = 10;
int redLevel = 20, greenLevel = 10, blueLevel = 40;

//	ink producer sleep time (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int producerSleepTime = 100000;

//	Enable this declaration if you want to do the traveler information
//	maintaining extra credit section
//TravelerInfo *travelList;



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

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render the grid.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawGrid(grid, NUM_ROWS, NUM_COLS);
	//
	//	Use this drawing call instead if you do the extra credits for
	//	maintaining traveler information
//	drawGridAndTravelers(grid, NUM_ROWS, NUM_COLS, travelList);
	
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

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawState(numLiveThreads, redLevel, greenLevel, blueLevel);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

//------------------------------------------------------------------------
//	These are the functions that would be called by a traveler thread in
//	order to acquire red/green/blue ink to trace its trail.
//	You *must* synchronized access to the ink levels
//------------------------------------------------------------------------
//
int acquireRedInk(int theRed)
{
	int ok = 0;
	if (redLevel >= theRed)
	{
		redLevel -= theRed;
		ok = 1;
	}
	return ok;
}

int acquireGreenInk(int theGreen)
{
	int ok = 0;
	if (greenLevel >= theGreen)
	{
		greenLevel -= theGreen;
		ok = 1;
	}
	return ok;
}

int acquireBlueInk(int theBlue)
{
	int ok = 0;
	if (blueLevel >= theBlue)
	{
		blueLevel -= theBlue;
		ok = 1;
	}
	return ok;
}

//------------------------------------------------------------------------
//	These are the functions that would be called by a producer thread in
//	order to refill the red/green/blue ink tanks.
//	You *must* synchronized access to the ink levels
//------------------------------------------------------------------------
//
int refillRedInk(int theRed)
{
	int ok = 0;
	if (redLevel + theRed <= MAX_LEVEL)
	{
		redLevel += theRed;
		ok = 1;
	}
	return ok;
}

int refillGreenInk(int theGreen)
{
	int ok = 0;
	if (greenLevel + theGreen <= MAX_LEVEL)
	{
		greenLevel += theGreen;
		ok = 1;
	}
	return ok;
}

int refillBlueInk(int theBlue)
{
	int ok = 0;
	if (blueLevel + theBlue <= MAX_LEVEL)
	{
		blueLevel += theBlue;
		ok = 1;
	}
	return ok;
}

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupProducers(void)
{
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * producerSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		producerSleepTime = newSleepTime;
	}
}

void slowdownProducers(void)
{
	//	increase sleep time by 20%
	producerSleepTime = (12 * producerSleepTime) / 10;
}

//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function
//------------------------------------------------------------------------
int main(int argc, char** argv)
{
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
	
	//	Now we can do application-level
	initializeApplication();

	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	//delete allocated memory
	for (int i=0; i<NUM_ROWS; i++) {
		free(grid[i]);
		free(locks[i]);
	}
	free(grid);
	free(locks);
	free(threads);
	free(colorLocks);
	return 0;
}


/**
initializeApplication sets up the grid, all of the traveler threads, and all of the locks for the threads to use. It instantiates all of the threads then terminates.
*/
void initializeApplication(void) {

	//initialize grid to black & initialize locks 
	grid = (int**) malloc(NUM_ROWS * sizeof(int*));
	locks = (pthread_mutex_t**)calloc(NUM_ROWS, sizeof(pthread_mutex_t*));
	for (int i=0; i<NUM_ROWS; i++) {
		grid[i] = (int*) malloc(NUM_COLS * sizeof(int));
		locks[i] = (pthread_mutex_t*)calloc(NUM_COLS, sizeof(pthread_mutex_t));
		for (int j=0; j<NUM_COLS; j++) {
			grid[i][j] = 0xFF000000;
			pthread_mutex_init(&locks[i][j], NULL);
		}
	}
	colorLocks = (pthread_mutex_t*)calloc(3, sizeof(pthread_mutex_t));
	for (int i=0; i<3; i++) {
			pthread_mutex_init(&colorLocks[i], NULL);
	}


	//initalize threads
	int numThreads = 8;
	threads = (ThreadInfo*)calloc(numThreads, sizeof(ThreadInfo));
	for (int i = 0; i < numThreads; i++) {
		threads[i].index = i;

		//initialize traveller
		TravelerInfo* data = (TravelerInfo*)calloc(1, sizeof(TravelerInfo));
		data->grid = grid;
		data->index = i;
		int errCode = pthread_create(&threads[i].threadID, NULL, traveller, (void*)data);
		if (errCode != 0) exit(0);
	}
}

/**
traveller
The traveller function contains the information for an instance of a traveller thread. WHen executed, the traveller will generate a color and a random location on a grid, then paint a path of its color on the grid until the it reaches a corner. At that point, the function terminates.

@param param is the traveller struct containing the index and grid
@return NULL is the value the traveller function returns
*/
void* traveller(void* param) {

	//start traveller
	TravelerInfo* data = (TravelerInfo*)param;
	srand((unsigned int)data->index);
	data->type = (TravelerType)(rand() % 3);
	data->row = rand() % NUM_ROWS;
	data->col = rand() % NUM_COLS;
	data->dir = (TravelDirection)(rand() % 4);
	data->isLive = 1;
	numLiveThreads++;

	//acquire ink
	pthread_mutex_lock(&colorLocks[(int)data->type]);
	if (data->type == RED_TRAV) while (acquireRedInk(1) == 0);
	else if (data->type == BLUE_TRAV) while (acquireBlueInk(1) == 0);
	else while (acquireGreenInk(1) == 0);
	pthread_mutex_unlock(&colorLocks[(int)data->type]);

	//add color to origin square
	unsigned char* colors;
	pthread_mutex_lock(&locks[data->row][data->col]);
	colors = (unsigned char*)&data->grid[data->row][data->col];
	if (data->type == RED_TRAV) colors[0] = 0xFF;
	else if (data->type == GREEN_TRAV) colors[1] = 0xFF;
	else colors[2] = 0xFF;
	pthread_mutex_unlock(&locks[data->row][data->col]);

	//loop until in a corner
	bool corner[2] = { false };
	int dest;
	while (!(corner[0] && corner[1])) {
	
		//randomly change direction
		int x = (int)data->dir;
		if (rand() % 2 == 0) x += 1; 
		else x += 3;
		x = x % 4;

		//if next to and facing an edge, flip direction.
		if ((NUM_ROWS-data->row == 1) && (x == 0)) x = 2;
		else if ((data->row == 0) && (x == 2)) x = 0;
		else if ((NUM_COLS-data->col == 1) && (x == 1)) x = 3;
		else if ((data->col == 0) && (x == 3)) x = 1;	
		data->dir = (TravelDirection)x;

		//pretermine some path in range
		if (data->dir == NORTH) {
			dest = data->row + (rand() % (NUM_ROWS - data->row));

			//paint path NORTH
			while (data->row != dest) {
				data->row++;

				//acquire ink
				pthread_mutex_lock(&colorLocks[(int)data->type]);
				if (data->type == RED_TRAV) while (acquireRedInk(1) == 0);
				else if (data->type==BLUE_TRAV) while (acquireBlueInk(1)==0);
				else while (acquireGreenInk(1) == 0);
				pthread_mutex_unlock(&colorLocks[(int)data->type]);

				//add color to square
				pthread_mutex_lock(&locks[data->row][data->col]);
				colors = (unsigned char*)&data->grid[data->row][data->col];
				if (data->type == RED_TRAV) colors[0] = 0xFF;
				else if (data->type == GREEN_TRAV) colors[1] = 0xFF;
				else colors[2] = 0xFF;
				pthread_mutex_unlock(&locks[data->row][data->col]);	 

			}
		} else if (data->dir == SOUTH) {
			dest = data->row - (rand() % (data->row+1));

			//paint path SOUTH
			while (data->row != dest) {
				data->row--;

				//acquire ink
				pthread_mutex_lock(&colorLocks[(int)data->type]);
				if (data->type == RED_TRAV) while (acquireRedInk(1) == 0);
				else if (data->type==BLUE_TRAV) while (acquireBlueInk(1)==0);
				else while (acquireGreenInk(1) == 0);
				pthread_mutex_unlock(&colorLocks[(int)data->type]);


				//add color to block
				pthread_mutex_lock(&locks[data->row][data->col]);
				colors = (unsigned char*)&data->grid[data->row][data->col];
				if (data->type == RED_TRAV) colors[0] = 0xFF;
				else if (data->type == GREEN_TRAV) colors[1] = 0xFF;
				else colors[2] = 0xFF;
				pthread_mutex_unlock(&locks[data->row][data->col]);	 	

			}
		} else if (data->dir == WEST) {
			dest = data->col + (rand() % (NUM_COLS - data->col));

			//paint path WEST
			while (data->col != dest) {
				data->col++;

				//acquire ink
				pthread_mutex_lock(&colorLocks[(int)data->type]);
				if (data->type == RED_TRAV) while (acquireRedInk(1) == 0);
				else if (data->type==BLUE_TRAV) while (acquireBlueInk(1)==0);
				else while (acquireGreenInk(1) == 0);
				pthread_mutex_unlock(&colorLocks[(int)data->type]);

				//add color to block
				pthread_mutex_lock(&locks[data->row][data->col]);
				colors = (unsigned char*)&data->grid[data->row][data->col];
				if (data->type == RED_TRAV) colors[0] = 0xFF;
				else if (data->type == GREEN_TRAV) colors[1] = 0xFF;
				else colors[2] = 0xFF;
				pthread_mutex_unlock(&locks[data->row][data->col]);	 	

			}
		} else {
			dest = data->col - (rand() % (data->col+1));

			//paint path EAST
			while (data->col != dest) {
				data->col--;

				//acquire ink
				pthread_mutex_lock(&colorLocks[(int)data->type]);
				if (data->type == RED_TRAV) while (acquireRedInk(1) == 0);
				else if (data->type==BLUE_TRAV) while (acquireBlueInk(1)==0);
				else while (acquireGreenInk(1) == 0);
				pthread_mutex_unlock(&colorLocks[(int)data->type]);

				//add color to block
				pthread_mutex_lock(&locks[data->row][data->col]);
				colors = (unsigned char*)&data->grid[data->row][data->col];
				if (data->type == RED_TRAV) colors[0] = 0xFF;
				else if (data->type == GREEN_TRAV) colors[1] = 0xFF;
				else colors[2] = 0xFF;
				pthread_mutex_unlock(&locks[data->row][data->col]);	
	 
			}
		}

		//check for corners
		if ((NUM_ROWS-data->row<=1) || (data->row==0)) corner[0] = true;
		else corner[0] = false;
		if ((NUM_COLS-data->col<=1) || (data->col==0)) corner[1] = true;		
		else corner[1] = false;
		
	}
	//delete thread
	data->isLive = 0;
	numLiveThreads--;
	delete data;
	return NULL;
}


