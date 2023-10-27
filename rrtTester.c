#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "obstacles.h"
#include "display.h"



// This program simulates the creation of rapidly-exploring random trees in a polygonal environment.
// This program should take in three command-line arguments which are:
// (1) the edge growth length for each tree branch and
// (2) the maximum number of tree nodes to create
// (3) the environment number to test

int main(int argc, char *argv[]) {
  	Environment   environment;
  	char		  environmentNumber;
  	
  	srand(time(NULL));	// Set the seed for random numbers
  	
  	/*******************************************************************************************/
  	/* Replace the three lines below with numbers that are read in from command line arguments */
    /*******************************************************************************************/
	if(argc != 4 ){
		printf("Please enter 4 arguments, 1. the growth amount 2. maximum number of nodes 3. type of enviroment from 1-5\n");
		exit(-1);
	}
	if(atoi(argv[1]) < 5 || atoi(argv[1])> 100){
		printf("The entered growth amount is invalid\n");
		exit(-1);
	}if(atoi(argv[2])< 5 || atoi(argv[2])> 5000){
		printf("The entered maximum number of nodes is invalid\n");
		exit(-1);
	}if(atoi(argv[3])< 1 || atoi(argv[3])> 5){
		printf("The Enviroment Number does not exist\n");
		exit(-1);
	}
	else{
		environmentNumber = atoi(argv[3]);
		environment.growthAmount = atoi(argv[1]);
		environment.maximumNodes = atoi(argv[2]);
	}


	/********************************************************/
	/* YOU MUST NOT CHANGE ANY CODE BENEATH THIS COMMENT !! */
	/********************************************************/

	// Open a window for display
	initializeWindow();

	// Initialize the environment and add some obstacles
	short int *xsp, *ysp, *wsp, *hsp;
	environment.startX   = 30;
	environment.startY   = 30;
	environment.maximumX = ENV_WIDTH;
	environment.maximumY = ENV_HEIGHT;
	environment.rrt      = NULL;
	environment.path     = NULL;
	switch (environmentNumber) {
		case 1: {
			short xs[] = {  0,  20, 720,  20, 120, 280, 580, 320,  80};
			short ys[] = {540, 540, 540,  20, 460, 420, 340, 280, 300};
			short ws[] = { 20, 700,  20, 700,  60, 220,  60,  40, 160};
			short hs[] = {540,  20, 540,  20,  80, 100, 200, 180, 110};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 9;
			break; }
		case 2: {
			short xs[] = {  0,  20, 720,  20,  60, 200, 355, 600, 120, 280};
			short ys[] = {540, 540, 540,  20, 480, 400, 480, 200, 200, 240};
			short ws[] = { 20, 700,  20, 700, 385, 480,  35,  20,  20,  40};
			short hs[] = {540,  20, 540,  20,  30,  45, 225,  20,  20, 220};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 10;
			break; }
		case 3: {
			short xs[] = {  0,  20, 720,  20,  20, 500, 480, 180, 360,  80};
			short ys[] = {540, 540, 540,  20, 520, 520, 160, 200, 480, 260};
			short ws[] = { 20, 700,  20, 700, 100, 220, 140,  50,   2, 400};
			short hs[] = {540,  20, 540,  20, 100, 160,  50, 130, 400,   2};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 10;
			break; }
		case 4: {
			short xs[14*10+4] = {  0,  20, 720,  20};
			short ys[14*10+4] = {540, 540, 540,  20};
			short ws[14*10+4] = { 20, 700,  20, 700};
			short hs[14*10+4] = {540,  20, 540,  20};
			for (int i=0; i<14; i++)
				for (int j=0; j<10; j++) {
					xs[i*10+j+4] = 50*(i+1);
					ys[i*10+j+4] = 50*(j+1);
					ws[i*10+j+4] = 20;
					hs[i*10+j+4] = 20;
				}
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 4+14*10;
			break; }
		case 5: {
			short xs[] = {  0,  20, 720,  20,  70};
			short ys[] = {540, 540, 540,  20, 470};
			short ws[] = { 20, 700,  20, 700, 600};
			short hs[] = {540,  20, 540,  20, 400};
			xsp = xs; ysp = ys, wsp = ws, hsp = hs;
			environment.numObstacles = 5;
			break; }
		default:
			environment.numObstacles = 0;
	}
	environment.obstacles = (Obstacle *)malloc(environment.numObstacles*sizeof(Obstacle));
	for (int i=0; i<environment.numObstacles; i++) {
		environment.obstacles[i].x = xsp[i];
		environment.obstacles[i].y = ysp[i];
		environment.obstacles[i].w = wsp[i];
		environment.obstacles[i].h = hsp[i];
	}

	// Create the RRT
	createRRT(&environment);

	// Trace back the path from the top left to the start
	tracePath(&environment, environment.maximumX - 30, environment.maximumY-30);

	// Display the environment then wait for user to press ENTER
	displayEnvironment(&environment);
	getchar(); 

	// Cleanup by freeing all allocated memory
	cleanupEverything(&environment);

	// Close the environment window
	closeWindow();
}
