#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "obstacles.h"
#include "display.h"
 
void isInObstacle(Environment *env, int *randPt,TreeNode *c,TreeNode **p);
void cNode(Environment *env,TreeNode *t,TreeNode **c,int randPt[]);
void closestNode(Environment *env,TreeNode **t,int *randPt);
double distanceCalc(TreeNode *t,int *randPt, Environment *env);
void linkedFree(Child *node);
void linkedPathFree(TreeNode *node);
void linkedzRRTFree(Child *node);

// This is a helper function that determines whether or not line segment (v1---v2) intersects line segment (v3---v4)
unsigned char linesIntersect(short v1x, short v1y, short v2x, short v2y, short v3x, short v3y, short v4x, short v4y) {
  	float uA = ((v4x-v3x)*(v1y-v3y) - (v4y-v3y)*(v1x-v3x)) / (float)(((v4y-v3y)*(v2x-v1x) - (v4x-v3x)*(v2y-v1y)));
  	float uB = ((v2x-v1x)*(v1y-v3y) - (v2y-v1y)*(v1x-v3x)) / (float)(((v4y-v3y)*(v2x-v1x) - (v4x-v3x)*(v2y-v1y)));

  	// If uA and uB are between 0-1, there is an intersection
  	if (uA > 0 && uA < 1 && uB > 0 && uB < 1) 
		return 1;

	return 0;
}




// Create a rrt using the growthAmount and maximumNodes parameters in the given environment.
void createRRT(Environment *env) {	
	int randPt[2] = {0,0};
	env->rrt = (TreeNode **) malloc(sizeof(TreeNode*)*(env->maximumNodes));
	if (env->rrt == NULL) {
		printf("error: could not allocate memory for the rrt array \n");
		return;
	}
	env->numNodes = 0;
	TreeNode *p = NULL;
	p = (TreeNode *) malloc(sizeof(TreeNode));
	if (p == NULL) {
		printf("error: could not allocate memory for the nodeHead \n");
		return;
	}
	p->x = env->startX;
	p->y = env->startY;
	p->parent = NULL;
	p->firstChild = NULL;
	env->rrt[0] = p;
	env->numNodes++;
  //Loops thorough the rrt array adding nodes until it reaches the max
	for(int i = 1; i < env->maximumNodes-1; i++)
	{
		TreeNode *c;
		c = (TreeNode *) malloc(sizeof(TreeNode));
		if (c == NULL) {
			printf("error: could not allocate memory for the node \n");
			return;
		}
		TreeNode *t;
		Child *new;
		new = (Child *) malloc(sizeof(Child));
		if (new == NULL) {
			printf("error: could not allocate memory for the child node \n");
			return;
		}
		new->nextSibling = NULL;
		isInObstacle(env,randPt,c,&t);
		new->node = c;
		env->rrt[i] = c;
		env->numNodes++;
		c->parent = t;
		if(t->firstChild == NULL){
			t->firstChild = new;
		}else{
;			Child *sib = t->firstChild;
			while(sib->nextSibling != NULL){
				sib = sib->nextSibling;
			}
			sib->nextSibling = new;




		}



	}
	

}


// Trace the path back from the node that is closest to the given (x,y) coordinate to the root
void tracePath(Environment *env, unsigned short x, unsigned short y) {	
	int randPt[2] = {(int)x,(int)y};
	TreeNode *n;
	closestNode(env,&n,randPt);
	int counter = 1;
	int index = 1;
	while (n != env->rrt[0]) {
		TreeNode  *p = n->parent;
		n = p;
		counter++;
	}
	env->path = (TreeNode **) malloc(sizeof(TreeNode*)*counter);
	if (env->path == NULL) {
		printf("error: could not allocate memory for the path array \n");
		return;
	}
	TreeNode *p = NULL;
	closestNode(env,&p,randPt);
	env->path[0] = p;
	TreeNode *temp = env->path[0];
	while (temp != env->rrt[0]) {
		TreeNode  *p = temp->parent;
		temp = p;
		env->path[index] = temp;
		index++;

	}

}


// This procedure cleans up everything by creeing all alocated memory
void cleanupEverything(Environment *env){
	for (int i=0; i<env->numNodes; i++) {
		TreeNode  *n = env->rrt[i];
		if (env->rrt[i]->firstChild != NULL){
				linkedFree(env->rrt[i]->firstChild);
		}
		free(env->rrt[i]);

	}
	free(env->path);
	free(env->obstacles);
	free(env->rrt);
}
/**************************************************************/
/* checks if the randomly creates a randomly generated node and checks if it follos the restriction else it will make another one
 
 input/output
 env -Pointer to enviroment object
 randPt - pointer to an array that will hold the randmoly created point
 c - the soon to be created node that is passed by refrence 
 p - an empty pointer that will point to the closest node to the newly created node
 

 */
void isInObstacle(Environment *env, int *randPt,TreeNode *c,TreeNode **p){
	int flag = 0;
	int x;
	int y;
	while(flag == 0){
		x = rand() % (env->maximumX) + env->startX;
		y = rand() % (env->maximumY) + env->startY;
		randPt[0] = x;
		randPt[1] = y;
		closestNode(env,p,randPt);
		cNode (env,*p,&c,randPt);
		if(c->x > env->maximumX ||c->y > env->maximumY || c->y < env->startY || c->x < env->startX){
			flag = 0;
			continue;
			}
		for(int i = 0; i < env->numObstacles; i++){
			flag = 1;
      //checks if randomly generated node is in an obstacle
			if(!(c->x <= env->obstacles[i].x+env->obstacles[i].w && c->x >= env->obstacles[i].x && c->y <= env->obstacles[i].y && c->y >= env->obstacles[i].y-env->obstacles[i].h)){
        //this array contains all the calculation to determine the location of each line of the lines of all the obstacles 
					short temp[4][4] = {{env->obstacles[i].x,env->obstacles[i].y,env->obstacles[i].x+env->obstacles[i].w,env->obstacles[i].y},
									{env->obstacles[i].x,env->obstacles[i].y,env->obstacles[i].x,env->obstacles[i].y-env->obstacles[i].h},
								{env->obstacles[i].x+env->obstacles[i].w,env->obstacles[i].y,env->obstacles[i].x+env->obstacles[i].w,env->obstacles[i].y-env->obstacles[i].h},
									{env->obstacles[i].x,env->obstacles[i].y-env->obstacles[i].h,env->obstacles[i].x+env->obstacles[i].w,env->obstacles[i].y-env->obstacles[i].h}};
				for(int j = 0; j < 4; j++ ){
					if(linesIntersect((*p)->x,(*p)->y, c->x,c->y, temp[j][0],temp[j][1],temp[j][2], temp[j][3]) == 0){
						continue;
					}else{
						flag = 0;
						break;
					}
				}
			}else{
				flag = 0;
				break;
			}
			if(flag == 0){
				break;
			}
		}
	}
}

/**************************************************************/
/* finds the closest node to the rand point 
 
 input/output
 env -Pointer to enviroment object
 randPt - pointer to an array that will hold the randmoly created point
 t - pointer to the closest point that will be pased by refrence and returned
 

 */
void closestNode(Environment *env,TreeNode **t,int *randPt){
	TreeNode *p = env->rrt[0];
	for(int i = 0; i < env->numNodes-1; i++){
		TreeNode *tester = env->rrt[i];
		if(distanceCalc(p,randPt,env) > distanceCalc(tester,randPt,env)){
			p = tester;
		}
	}
	*t = p;
}
//calculates the distance between 2 points
double distanceCalc(TreeNode *t,int *randPt,Environment *env){
	return sqrt(pow(t->x-(double)randPt[0],2)+pow(t->y-(double)randPt[1],2));
}

/**************************************************************/
/* creates the new node
 
 input/output
 env -Pointer to enviroment object
 randPt - pointer to an array that will hold the randmoly created point
 c - the soon to be created node that is passed by refrence 
 t - an empty pointer that will point to the closest node to the newly created node
 

 */
void cNode(Environment *env,TreeNode *t,TreeNode **c,int randPt[]){
	double angle = atan2(randPt[1]-(t->y),randPt[0]-(t->x));
	(*c)->x = (t->x)+cos(angle)*env->growthAmount;
	(*c)->y = (t->y)+sin(angle)*env->growthAmount;
	(*c)->parent = NULL;
	(*c)->firstChild = NULL;
}
//frees all the nodes in the linked list of Child nodes
void linkedFree(Child *node){
	Child* temp;
	while(node != NULL){
		temp = node;
		node = node->nextSibling;
		free(temp);
	}
		
}


