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
std::chrono::milliseconds timespan(500);
void* robotFunc(void* param) {

	//start robot, lox robot square, lock box square, wait for other robots
	struct robotThread* t = (struct robotThread*)param;

	pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY][t->robox->robX]);
	pthread_mutex_lock(&t->grid->gridlocks[t->robox->boxY][t->robox->boxX]);

	t->map[t->robox->boxY][t->robox->boxX] = true;
	t->map[t->robox->robY][t->robox->robX] = true;

	std::this_thread::sleep_for(timespan);
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
			return true;

		//if box left of dest move east
		} else if (t->robox->boxX < dest[1]) {

//if the square is occupied, wait to see if intersection or deadlock
			int safe = 2;
			
			while ((t->map[t->robox->boxY][t->robox->boxX+1])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			pthread_mutex_lock(&t->grid->gridlocks[t->robox->boxY][t->robox->boxX+1]);
			t->map[t->robox->boxY][t->robox->boxX+1] = true;
			t->robox->boxX++;
			t->robox->robX++;
			t->map[t->robox->robY][t->robox->robX-1] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY][t->robox->robX-1]);
			write(t, name, 'E');

		//if box right of dest move west
		} else {
//if the square is occupied, wait to see if intersection or deadlock
			int safe = 2;
			while ((t->map[t->robox->boxY][t->robox->boxX-1])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			pthread_mutex_lock(&t->grid->gridlocks[t->robox->boxY][t->robox->boxX-1]);
			t->map[t->robox->boxY][t->robox->boxX-1] = true;
			t->robox->boxX--;
			t->robox->robX--;
			t->map[t->robox->robY][t->robox->robX-1] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY][t->robox->robX+1]);
			write(t, name, 'W');
		}

	//box not in door row
	} else {

		//if box below dest move north
		if (t->robox->boxY < dest[0]) {
//if the square is occupied, wait to see if intersection or deadlock
			int safe = 2;
			while ((t->map[t->robox->boxY+1][t->robox->boxX])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			//push
			pthread_mutex_lock(&t->grid->gridlocks[t->robox->boxY+1][t->robox->boxX]);
			t->map[t->robox->boxY+1][t->robox->boxX] = true;
			t->robox->boxY++;
			t->robox->robY++;
			t->map[t->robox->robY-1][t->robox->robX] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY-1][t->robox->robX]);
			write(t, name, 'N');

		//if box above dest move south
		} else {
//if the square is occupied, wait to see if intersection or deadlock
			int safe = 2;
			while ((t->map[t->robox->boxY-1][t->robox->boxX])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);
			

			//push
			pthread_mutex_lock(&t->grid->gridlocks[t->robox->boxY-1][t->robox->robX]);
			t->map[t->robox->boxY-1][t->robox->boxX] = true;
			t->robox->boxY--;
			t->robox->robY--;
			t->map[t->robox->robY+1][t->robox->robX] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY+1][t->robox->robX]);
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
		
		//if left of dest move east
		if (t->robox->robX < dest[1]) {

			int safe = 3;
			while ((t->map[t->robox->robY][t->robox->robX+1])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY][t->robox->robX+1]);
			t->map[t->robox->robY][t->robox->robX+1] = true;
			t->robox->robX++;
			t->map[t->robox->robY][t->robox->robX-1] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY][t->robox->robX-1]);
			write(t, name, 'E');

		//if right of dest move west
		} else {

			
			int safe = 3;
			while ((t->map[t->robox->robY][t->robox->robX-1])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY][t->robox->robX-1]);
			t->map[t->robox->robY][t->robox->robX-1] = true;
			t->robox->robX--;
			t->map[t->robox->robY][t->robox->robX+1] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY][t->robox->robX+1]);
			write(t, name, 'W');
		}
		
	//if not in row, check flag
	//If flag, move horizontally away from the door
	//to circumvent the box
	} else if (flag) {

		//if right of dest move east
		if (t->robox->robX < dest[1]) {

//if the square is occupied, wait to see if intersection or deadlock
			int safe = 3;
			while ((t->map[t->robox->robY][t->robox->robX+1])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY][t->robox->robX+1]);
			t->map[t->robox->robY][t->robox->robX+1] = true;
			t->robox->robX++;
			t->map[t->robox->robY][t->robox->robX-1] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY][t->robox->robX-1]);
			write(t, name, 'E');

		//if left of dest move west
		} else {

			
//if the square is occupied, wait to see if intersection or deadlock
			int safe = 3;
			while ((t->map[t->robox->robY][t->robox->robX-1])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);
			
			pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY][t->robox->robX-1]);
			t->map[t->robox->robY][t->robox->robX-1] = true;
			t->robox->robX--;
			t->map[t->robox->robY][t->robox->robX+1] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY][t->robox->robX+1]);
			write(t, name, 'W');
		}

	//if not in dest row and not flag, move toward the box
	} else {

		//if below dest move north
		if (t->robox->robY < dest[0]) {

//if the square is occupied, wait to see if intersection or deadlock
			int safe = 3;
			while ((t->map[t->robox->robY+1][t->robox->robX])and(safe)){
				//wait
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);

			pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY+1][t->robox->robX]);
			t->map[t->robox->robY+1][t->robox->robX] = true;
			t->robox->robY++;
			t->map[t->robox->robY-1][t->robox->robX] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY-1][t->robox->robX]);write(t, name, 'N');

		//if above dest move south
		} else {

			//if the square is occupied, 
			//wait to see if intersection or deadlock
			int safe = 3;
			while ((t->map[t->robox->robY-1][t->robox->robX])and(safe)){
				//wait a cycle
				std::this_thread::sleep_for(timespan);
				safe--;
			}
			if (!safe) resolveDeadlock(t);
						
			pthread_mutex_lock(&t->grid->gridlocks[t->robox->robY-1][t->robox->robX]);
			t->map[t->robox->robY-1][t->robox->robX] = true;
			t->robox->robY--;
			t->map[t->robox->robY+1][t->robox->robX] = false;
			pthread_mutex_unlock(&t->grid->gridlocks[t->robox->robY+1][t->robox->robX]);
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

void resolveDeadlock(robotThread* t) {

	while (true) {
		//check deadlock flags
		int x = 0;
		pthread_mutex_lock(&t->grid->DLlock);
		while (x < t->grid->numBots) {
			//if a deadlock flag is set
			//we are deadlocked,
			//if that flag is this bot
			//stationary obstacle
			if ((t->grid->deadlocked[x]) and (x==t->threadIndex)){

				
				pthread_mutex_lock(&t->grid->filelock);
				fprintf(t->grid->fp,"robot %d became deadlocked with robot %d\n",t->threadIndex,x);
				pthread_mutex_unlock(&t->grid->filelock);

				// code to resolve the dead lock

				t->grid->deadlocked[x] = false;
				pthread_mutex_unlock(&t->grid->DLlock);
				return;
			
			//if the flag is not this bot,
			//we are in the path
			//of another bot
			} else if (t->grid->deadlocked[x]) {


				pthread_mutex_lock(&t->grid->filelock);
				fprintf(t->grid->fp,"robot %d became deadlocked with a stationary object\n",t->threadIndex);
				pthread_mutex_unlock(&t->grid->filelock);

				// code to resolve the dead lock

				t->grid->deadlocked[x] = false;
				pthread_mutex_unlock(&t->grid->DLlock);
				return;
			}
			x++;
		}
		//if we checked every flag,
		//set this bots flag
		if (x==t->grid->numBots){
		 	t->grid->deadlocked[t->threadIndex] = true;
		}
		pthread_mutex_unlock(&t->grid->DLlock);
	}
	
}
