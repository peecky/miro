#ifndef __AnoPi_MIRO_H__
#define __AnoPi_MIRO_H__

#define	up		0
#define	down	1
#define right	2
#define left	3
#define NOT_ANY_DIRECTION 4	// cannot go any
#define NOTUP	4	// cannot go up
#define NOTDOWN	5	// cannot go down
#define NOTRIGHT 6	// cannot go right
#define NOTLEFT	7	// cannot go left



#define INIT_TIMEFACTOR 30

typedef struct Cell{
	bool is_open;	// has this cell visited for making maze
	bool road[4];	// is each four directions of cells(up, down, right, left) connected to this cell

	Cell() {
		is_open = false;
		road[0] = false;
		road[1] = false;
		road[2] = false;
		road[3] = false;
	}
} Cell;

void draw_maze();
void reviewpoint(bool draw = false);

#endif
