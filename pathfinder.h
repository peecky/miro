#ifndef __AnoPi_PATHFINDER_H__
#define __AnoPi_PATHFINDER_H__

#define Body 4
#define Arm  1
#define Leg  2
#define Eye  3

#define ROLL_FACT	10

class PathFinder {
public:
	enum Direction { UP = 0, DOWN = 1, RIGHT = 2, LEFT = 3 };
	PathFinder(int x_position, int y_position, int maze_width, int maze_height);
	~PathFinder() { delete recursion_stack; }

	bool isMoving() { return ismoving; }	// is it doing moving animation?
	void set_dest( Direction new_dest );
	void Move();
	void SetBodyColor(double r, double g, double b)	{ bodyColorR = r; bodyColorG = g, bodyColorB = b; }
	void Draw();
	void UpdateStatus();
	// if animation is false, it doesn't move arm, leg and eye. Just draw
	// if get_goal is true then do goal ceremony

	double CurrentX() { return current_x; }	// return x position of the finder
	double CurrentY() { return current_y; }	// return y position of the finder
	void Set_getgoal() { get_goal = true; }

	/* stack of path finding */
	bool isStack_Empty() { return stack_top == -1; }
	void Stack_Push(int dest) { recursion_stack[++stack_top] = dest; }
	int Stack_Pop() { return (stack_top == -1)? recursion_stack[stack_top+1]: recursion_stack[stack_top--]; }
	int Stack_Top() { return recursion_stack[stack_top]; }
private:
	/* position and direction for path finding */
	double current_x;
	double current_y;
	double old_x;
	double old_y;
	int Dest;
	int init_dest;
	bool get_goal;

	/* animation */
	void lists();	// call list of the finder
	bool ismoving;
	double degree_7;	// a degree of 7, used calculating movement of one frame of animation
	int walk_status;	// for waking animation
	int eye_status;		// for face animation
	int rolling_status;	// for rotate animation
	int goal_ceremony_status;	// for goal ceremony
	double bodyColorR;
	double bodyColorG;
	double bodyColorB;

	/* stack of path finding */
	int* recursion_stack;
	int stack_top;
};

#endif
