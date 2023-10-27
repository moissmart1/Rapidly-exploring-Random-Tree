#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>

#include "obstacles.h"
#include "display.h"

#define  VERTEX_RADIUS    5
#define  EDGE_THICKNESS   3

const int   OBSTACLE_COLOR = 0x7799FF;
const int   BORDER_COLOR = 0x000000;
const int   EDGE_COLOR = 0x000000;
const int   SELECTED_COLOR = 0x008800;
const int   VERTEX_COLOR = 0xDD0000;


//Display-related variables
Display *display;
Window   win;
GC       gc;


// Initialize and open the simulator window with size ENV_WIDTH x ENV_HEIGHT.
void initializeWindow() {
  // Open connection to X server
  display = XOpenDisplay(NULL);

  // Create a simple window, set the title and get the graphics context then
  // make is visible and get ready to draw
  win = XCreateSimpleWindow(display,  RootWindow(display, 0), 0, 0, ENV_WIDTH, ENV_HEIGHT, 0, 0x000000, 0xFFFFFF);
  XStoreName(display, win, "RRT Maker");
  gc = XCreateGC(display, win, 0, NULL);
  XMapWindow(display, win);
  XFlush(display);
  usleep(20000);  // sleep for 20 milliseconds.
}

// Close the display window
void closeWindow() {
  XFreeGC(display, gc);
  XUnmapWindow(display, win);
  XDestroyWindow(display, win);
  XCloseDisplay(display);
}


// Redraw all the obstacles and the edges and the vertices
void displayEnvironment(Environment *env) {
	Child  *c = NULL; // A child of the current node
	
	// Erase the background 
	XSetForeground(display, gc, 0xFFFFFF);
    XFillRectangle(display, win, gc, 0, 0, ENV_WIDTH, ENV_HEIGHT);

    // Draw all the obstacles
    for (int i=0; i<env->numObstacles; i++) {
		XSetForeground(display, gc, OBSTACLE_COLOR);
		XFillRectangle(display, win, gc, env->obstacles[i].x,ENV_HEIGHT-env->obstacles[i].y, 
						env->obstacles[i].w, env->obstacles[i].h);
		XSetForeground(display, gc, BORDER_COLOR);
		XDrawRectangle(display, win, gc, env->obstacles[i].x,ENV_HEIGHT-env->obstacles[i].y, 
						env->obstacles[i].w, env->obstacles[i].h);
    }
    
    // Draw the tree
    if (env->rrt != NULL) {
   		// Go through all the nodes of the rrt and draw edges from them to their children
    	XSetForeground(display, gc, EDGE_COLOR);
    	for (int i=0; i<env->numNodes; i++) {
    		TreeNode  *n = env->rrt[i];
    		c = n->firstChild;
			while (c != NULL) {
				XDrawLine(display, win, gc, n->x, ENV_HEIGHT-(n->y), c->node->x, ENV_HEIGHT-(c->node->y));
				c = c->nextSibling;
			}	
		}
	
    	// Draw all the Nodes
   		for (int i=0; i<env->numNodes; i++) {
    		TreeNode  *n = env->rrt[i];
			XSetForeground(display, gc, VERTEX_COLOR);
			XFillArc(display, win, gc, n->x-VERTEX_RADIUS, ENV_HEIGHT-(n->y+VERTEX_RADIUS),
				 	 2*VERTEX_RADIUS, 2*VERTEX_RADIUS, 0, 360*64);
			XSetForeground(display, gc, BORDER_COLOR);
			XDrawArc(display, win, gc, n->x-VERTEX_RADIUS, ENV_HEIGHT-(n->y+VERTEX_RADIUS),
				 	 2*VERTEX_RADIUS, 2*VERTEX_RADIUS, 0, 360*64);
		}
	}

    // Draw the path
    if (env->path != NULL) {
   		// Go through all the nodes of the path and draw edges
    	XSetForeground(display, gc, SELECTED_COLOR);
    	TreeNode *n = env->path[0];
    	while (n != env->rrt[0]) {
    		TreeNode  *p = n->parent;
    		// Check if the line is more horizontal than vertical
    		if (abs(n->x - p->x) > abs(n->y - p->y)) {
    			for (int y=-EDGE_THICKNESS; y<=EDGE_THICKNESS; y++)
    				XDrawLine(display, win, gc, n->x, ENV_HEIGHT-(n->y)+y, p->x, ENV_HEIGHT-(p->y)+y);
    		}
    		else {
   				for (int x=-EDGE_THICKNESS; x<=EDGE_THICKNESS; x++)
    				XDrawLine(display, win, gc, n->x+x, ENV_HEIGHT-(n->y), p->x+x, ENV_HEIGHT-(p->y));    		
    		}
			n = p;
		}
	
    	// Go through the nodes of the path and draw all the Nodes
    	n = env->path[0];
    	while (n != env->rrt[0]) {
			XSetForeground(display, gc, SELECTED_COLOR);
			XFillArc(display, win, gc, n->x-VERTEX_RADIUS, ENV_HEIGHT-(n->y+VERTEX_RADIUS),
				 	 2*VERTEX_RADIUS, 2*VERTEX_RADIUS, 0, 360*64);
			XSetForeground(display, gc, BORDER_COLOR);
			XDrawArc(display, win, gc, n->x-VERTEX_RADIUS, ENV_HEIGHT-(n->y+VERTEX_RADIUS),
				 	 2*VERTEX_RADIUS, 2*VERTEX_RADIUS, 0, 360*64);
			n = n->parent;
		}
	}

    XFlush(display);
    usleep(2000);
}

