#ifndef __AnoPi_PATHFINDER_H__
#define __AnoPi_PATHFINDER_H__

#define SHIFTFACTOR_X -10.0
#define SHIFTFACTOR_Y -11.5

#define Body 4
#define Arm  1
#define Leg  2
#define Eye  3

#define ROLL_FACT	10

class PathFinder {
public:
	PathFinder(int x_position = 0.0, int y_position = 0.0, double HEIGHT = 10.0, double WIDTH = 10.0);
	~PathFinder();

	bool isMoving();	// is it doing moving animation?
	void set_dest( int new_dest );
	void Move();
	void Draw(bool animation = true);
	// if animation is false, it doesn't move arm, leg and eye. Just draw
	// if get_goal is true then do goal ceremony

	double CurrentX();	// return x position of the finder
	double CurrentY();	// return y position of the finder
	void Set_getgoal();

	/* stack of path finding */
	bool isStack_Empty();
	void Stack_Push(int dest);
	int Stack_Pop();
	int Stack_Top();
private:
	/* position and direction for path finding */
	double current_x;
	double current_y;
	double old_x;
	double old_y;
	double height;	// height of finder
	double width;	// width of finder
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

	/* stack of path finding */
	int* recursion_stack;
	int stack_top;
};

#endif
