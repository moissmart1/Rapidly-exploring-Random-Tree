
// This represents a rectangular obstacle
typedef struct obst {
	short   	 x;
	short   	 y; 
	short   	 w;
	short   	 h;
} Obstacle;


// This represents a node of the tree
typedef struct node {
    short         x;				// Location of the node in the environment
    short         y;
    struct node  *parent;			// Parent of this node in the tree
    struct child *firstChild;		// The first child in a singly-linked list of children
} TreeNode;


// This represents a linked-list of children of a treenode
typedef struct child {
	TreeNode      *node;			// A child of a tree node
	struct child  *nextSibling;		// The next child (i.e., sibling) in the list of a node's children
} Child;



// This represents an environment with obstacles, a RRT and a path in the RRT
typedef struct {
	unsigned short  startX;			// The starting x of the RRT in the environment
	unsigned short  startY;			// The starting y of the RRT in the environment
	unsigned short  maximumX;		// The maximum X value in the environment
	unsigned short  maximumY;		// The maximum Y value in the environment
	unsigned short  maximumNodes;	// The maximum number of nodes to be added to the tree
	unsigned char   growthAmount;	// The amount (in pixels) to grow the tree each time
	Obstacle       *obstacles;		// rectangular obstacles in the environment
	unsigned short  numObstacles;	// # of obstacles
	TreeNode   	  **rrt;			// Array of all nodes in a A rapidly-exploring random tree
	unsigned short  numNodes;		// # of nodes currently in the rrt
	TreeNode	  **path;			// A computed path in the tree
} Environment;
	

// These are the external functions being used by the test program
extern void createRRT(Environment *);
extern void tracePath(Environment *, unsigned short, unsigned short);
extern void cleanupEverything(Environment *);

