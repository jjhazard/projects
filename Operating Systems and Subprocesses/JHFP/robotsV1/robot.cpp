#include <vector>
#include <pthread.h>
#include <iostream>
#include "robot.h"
/**
traveller
The traveller function contains the information for an instance of a traveller thread. WHen executed, the traveller will generate a color and a random location on a grid, then paint a path of its color on the grid until the it reaches a corner. At that point, the function terminates.

@param param is the traveller struct containing the index and grid
@return NULL is the value the traveller function returns
*/
void* robotFunc(void* param) {

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

	//flag to circumvent box
	bool flag = false;

	//robot loop
	while (t->isActive) {

		//push block
		if (pushable) {
			pushable = push(t, dest);
			
			//if we can't push, move to one side of the box
			if (!pushable) {

				//left or right side of box
				if (t->robox->boxX > t->robox->assignedDoor->doorX) {
					dest[0] = t->robox->boxX + 1;
				} else dest[0] = t->robox->boxX + 1;
				dest[1] = t->robox->boxY;

				//set flag
				flag = true;
			}

		//move block
		} else {
			pushable = move(t, dest, flag);
			flag = false;

			//if we can push, change destination to the door coordinates
			if (pushable) {
				dest[0] = t->robox->assignedDoor->doorX;
				dest[1] = t->robox->assignedDoor->doorY;
			}

		}
	}
	delete t;
	return NULL;
}

bool push(struct robotThread* t, int dest[2]) {

	char name[8] = "push";
	//if box in door row, check col
	if (t->robox->boxY == dest[1]) {

		//if robot not in door row, move robot
		if (t->robox->robY != dest[1]) return false;

		//if box in dest column, done
		if (t->robox->boxX == dest[0]) {
			t->isActive = false;
			return true;

		//if box left of dest move east
		} else if (t->robox->boxX < dest[0]) {
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
		if (t->robox->boxY < dest[1]) {
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

	//If in dest row
	if (t->robox->robY == dest[1]) {

		//if in dest column, push box
		if (t->robox->robX == dest[0]) return true;
		else {

			//if left of dest move east
			if (t->robox->robX < dest[0]) {
				t->robox->robX++;
				write(t, name, 'E');

			//if right of dest move west
			} else {
				t->robox->robX--;
				write(t, name, 'W');
			}
		}

	} else if (flag) {

		//if right of dest move east
		if (t->robox->robX < dest[1]) {

			//move
			t->robox->robX++;
			
			//write move
			write(t, name, 'E');

		//if left of dest move west
		} else {

			//move
			t->robox->robX--;

			//write move
			write(t, name, 'W');
		}

	//if not in dest row
	} else {

		//if below dest move north
		if (t->robox->robY < dest[1]) {
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
	
	fprintf(t->grid->fp,"Robot %d %s %c\n",t->threadIndex,func,dir);

}
