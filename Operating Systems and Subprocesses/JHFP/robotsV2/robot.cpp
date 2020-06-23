#include <vector>
#include <pthread.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "robot.h"
/**
traveller
The traveller function contains the information for an instance of a traveller thread. WHen executed, the traveller will generate a color and a random location on a grid, then paint a path of its color on the grid until the it reaches a corner. At that point, the function terminates.

@param param is the traveller struct containing the index and grid
@return NULL is the value the traveller function returns
*/
void* robotFunc(void* param) {

	std::chrono::milliseconds timespan(500);
	//start traveller
	struct robotThread* t = (struct robotThread*)param;

	//find destination square adjacent to box
	int dest[2] = { 0, 0 };
	if (t->robox->assignedDoor->doorY > t->robox->boxY) {
		dest[0] = t->robox->boxY-1;
	} else dest[0] = t->robox->boxY+1;
	dest[1] = t->robox->boxX;
	//flag to switch from moving to pushing
	bool pushable = false;
	//because all vertical movement happens first,
	//we need a flag value for when push returns
	//so we move around the box instead of through it
	bool flag = false;
	//robot loop
	while (t->isActive) {

		//push block
		if (pushable) {
			pushable = push(t, dest);
			
			//if we can't push, determine which side of the box to move to
			if (!pushable) {
				//left or right side of box
				if (t->robox->boxX > t->robox->assignedDoor->doorX) {
					dest[1] = t->robox->boxX + 1;
				} else dest[1] = t->robox->boxX - 1;
				dest[0] = t->robox->boxY;
				
				//set the flag
				flag = true;
			}
			std::this_thread::sleep_for(timespan);
		
		//move block
		} else {
			pushable = move(t, dest, flag);
			flag = false;
			//if we can push, change destination to the door coordinates
			if (pushable) {
				dest[0] = t->robox->assignedDoor->doorY;
				dest[1] = t->robox->assignedDoor->doorX;
			}
			std::this_thread::sleep_for(timespan);
		}
	}
	delete t;
	return NULL;
}

bool push(struct robotThread* t, int dest[2]) {

	char name[8] = "push";
	//if box in door row, check col
	if (t->robox->boxY == dest[0]) {

		//if robot not in door row, move robot
		if (t->robox->robY != dest[0]) return false;

		//if box in dest column, done
		if (t->robox->boxX == dest[1]) {
			t->isActive = false;

		//if box left of dest move east
		} else if (t->robox->boxX < dest[1]) {
			t->robox->boxX++;
			t->robox->robX++;
			write(t, name, 'E');

		//if box right of dest move west
		} else {
			t->robox->boxX--;
			t->robox->robX--;
			write(t, name, 'W');
		}

	//box not in door row
	} else {

		//if box below dest move north
		if (t->robox->boxY < dest[0]) {
			t->robox->boxY++;
			t->robox->robY++;
			write(t, name, 'N');

		//if box above dest move south
		} else {
			t->robox->boxY--;
			t->robox->robY--;
			write(t, name, 'S');
		}
	}

	//continue pushing
	return true;

}

bool move(struct robotThread* t, int dest[2], bool flag) {

	char name[8] = "move";

	//if in the same row as the destination, check column
	if (t->robox->robY == dest[0]) {

		//if in dest column, push box
		if (t->robox->robX == dest[1]) return true;
		else {

			//if left of dest move east
			if (t->robox->robX < dest[1]) {
				t->robox->robX++;
				write(t, name, 'E');

			//if right of dest move west
			} else {
				t->robox->robX--;
				write(t, name, 'W');
			}
		}

	//if not in row, check flag
	//If flag, move horizontally away from the door
	//to circumvent the box
	} else if (flag) {

		//if right of dest move east
		if (t->robox->robX < dest[1]) {
			t->robox->robX++;
			write(t, name, 'E');

		//if left of dest move west
		} else {
			t->robox->robX--;
			write(t, name, 'W');
		}

	//if not in dest row and not flag, move toward the box
	} else {

		//if below dest move north
		if (t->robox->robY < dest[0]) {
			t->robox->robY++;
			write(t, name, 'N');

		//if above dest move south
		} else {
			t->robox->robY--;
			write(t, name, 'S');
		}
	}

	//continue moving
	return false;
}


void write(struct robotThread* t, char* func, char dir) {
	
	pthread_mutex_lock(&t->grid->filelock);
	fprintf(t->grid->fp,"robot %d %s %c\n",t->threadIndex,func,dir);
	pthread_mutex_unlock(&t->grid->filelock);

}
