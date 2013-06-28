#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <ctime>
#include <sys/time.h>

#include "miro.h"
#include "pathfinder.h"

void display();
void reviewpoint();

Cell *cell;
int width, height;	// the size of maze
int starting_x, starting_y;	// starting position
int goal_x, goal_y;	// position of goal
double R, G, B;		// the color of background
int *chosen;		// the pointer of array of connected cells
bool work;			// making maze(true) or pause(false)
int state = 0;		// current state(making maze, finding path or end)
PathFinder* gb_finder = NULL;	// path finder object
bool Over_view = false;

int view_Left, view_Right, view_Bottom, view_Up;	//view points
int ViewFactor;
int ViewChange_x, ViewChange_y;
int timefactor;		// controls duration

static int timeGetTime() {
	static time_t init_sec = 0;
	timeval tv;
	gettimeofday(&tv, NULL);
	if (init_sec == 0) {
		init_sec = tv.tv_sec;
	}
	return (tv.tv_sec - init_sec) * 1000 + tv.tv_usec / 1000;
}

static inline Cell & cellXY(int x, int y) {
	return cell[y * width + x];
}

// remove the line between two connected cells
void erase_wall( int x, int y, int dest ){

	glColor3f( R, G, B );
	glBegin( GL_LINES );

	switch( dest ){

	case up:
		glVertex2f( (x+1)*10+0.02, (y+2)*10 );
		glVertex2f( (x+2)*10-0.02, (y+2)*10 );
		break;

	case down:
		glVertex2f( (x+1)*10+0.02, (y+1)*10 );
		glVertex2f( (x+2)*10-0.02, (y+1)*10 );
		break;

	case right:
		glVertex2f( (x+2)*10, (y+1)*10+0.02 );
		glVertex2f( (x+2)*10, (y+2)*10-0.02 );
		break;

	case left:
		glVertex2f( (x+1)*10, (y+1)*10+0.02 );
		glVertex2f( (x+1)*10, (y+2)*10-0.02 );
		break;
	}

	glEnd();
}

void draw_maze(){

	int i;
	int x, y;

	for( i = 0 ; i < width*height ; i++ ){
		x = i % width;
		y = i / width;

		if( cell[i].road[right] == true )	erase_wall( x, y, right );
		if( cell[i].road[up] ==true )		erase_wall( x, y, up );
		if( cell[i].road[down] == true )	erase_wall( x, y, down );
		if( cell[i].road[left] ==true )		erase_wall( x, y, left );
	}
}

// generate the maze
void gen_maze(){

	int x, y;	// position of the current cell
	int dest;	// direction of to be connected cell
	static int length = 0;	// size of the array chosen(store the visited(connected) cells)
	int tmp;
	int currTime;
	static int oldTime = 0;

	//if( work == false ) return;
	if( length == width * height) {
		state = 1;
		for(int i = 0; i < width*height; i++)
			cell[i].is_open = false;
		return;
	}

	// time
	currTime = timeGetTime();
	if(currTime - oldTime > timefactor*1)
		oldTime = currTime;
	else return;


	if( length == 0 ){
		// make start point and goal
		dest = rand()%2 + 1;
		if( dest == down ){
			// starting point
			starting_x = x = rand()%width;
			starting_y = y = height - 1;
			cellXY(x, y).road[up] = true;
			// goal
			goal_x = x = rand()%width;
			goal_y = y = 0;
			cellXY(x, y).road[down] = true;
		}
		else{
			// starting point
			starting_x = x = width - 1;
			starting_y = y = rand()%height;
			cellXY(x, y).road[right] = true;
			// goal
			goal_x = x = 0;
			goal_y = y = rand()%height;
			cellXY(x, y).road[left] = true;
		}
		chosen = new int [height * width];

		// choose the first cell randomly
		x = rand()%width;
		y = rand()%height;
		cellXY(x, y).is_open = true;
		chosen[0] = width*y + x;	// store the first visited cell

		length = 1;
	}

	bool cellOpen = false;
	while (!cellOpen) {
		tmp = chosen[ rand()%length ];	// randomly choose a cell which is visited
		x = tmp % width;
		y = tmp / width;

		dest = rand()%4;	// choose a direction to connect a nearby cell
		// if the cell on the direction has visited or is out of range
		// then select a cell and a directon again
		// if not, connect two cells and store the new cell(the cell on the direction)
		// to the array chosen(visited cells)
		switch( dest ){

		case up:
			if( y == height-1 || cellXY(x, y + 1).is_open == true)
				continue;

			cellXY(x, y + 1).is_open = true;

			cellXY(x, y + 1).road[ down ] = true;
			cellXY(x, y).road[ up ] = true;

			chosen[length] = width*( y + 1 ) + x;
			length++;
			cellOpen = true;
			break;

		case down:
			if( y == 0 || cellXY(x, y - 1).is_open == true )
				continue;

			cellXY(x, y - 1).is_open = true;

			cellXY(x, y - 1).road[ up ] = true;
			cellXY(x, y).road[ down ] = true;

			chosen[length] = width*(y - 1) + x;
			length++;
			cellOpen = true;
			break;

		case right:
			if( x == width-1 || cellXY(x + 1, y).is_open == true )
				continue;

			cellXY(x + 1,  y).is_open = true;

			cellXY(x + 1,  y).road[ left ] = true;
			cellXY(x,  y).road[ right ] = true;

			chosen[length] = width * y + x + 1;
			length++;
			cellOpen = true;
			break;

		case left:
			if( x == 0 || cellXY(x - 1,  y).is_open == true )
				continue;

			cellXY(x - 1,  y).is_open = true;

			cellXY(x - 1,  y).road[ right ] = true;
			cellXY(x,  y).road[ left ] = true;

			chosen[length] = width * y + x - 1;
			length++;
			cellOpen = true;
			break;
		}
	}
}

void reshape( int w, int h ){
	int size = ( width > height )? width : height;
	double move = ( width > height )? ( width-height )/2.0 : ( height-width )/2.0;

	glViewport(0, 0, w, h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	if( state == 0 ){
		if( width == size ){
			view_Left = 0.0;
			view_Right = 20 + size*10;
			view_Bottom = 0.0 - move*10;
			view_Up = size*10 + 20 - move*10;
		}
		else{
			view_Left = 0.0 - move*10;
			view_Right = 20 + size*10 - move*10;
			view_Bottom = 0.0;
			view_Up = size*10 + 20;
		}
	}

	gluOrtho2D( view_Left, view_Right, view_Bottom, view_Up );

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void display()
{
	double x;

	glClearColor( R, G, B, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glColor3f( 1-R, 1-G, 1-B );	// the color is the negative of background color

	// draw default(unmaden) maze
	glBegin( GL_LINES );
	for( x = 1 ; x < width+2 ; x++ ){
		glVertex2f( x*10, 10.0 );
		glVertex2f( x*10, height*10+10.0 );
	}
	for( x = 1 ; x < height+2; x++ ){
		glVertex2f( 10.0 , x*10 );
		glVertex2f( width*10+10.0 , x*10 );
	}
	glEnd();

	draw_maze();

	if(gb_finder != NULL) {
		const double SHIFTFACTOR_X = -10.0;
		const double SHIFTFACTOR_Y = -11.5;

		glLoadIdentity();
		glTranslatef(gb_finder->CurrentX() + SHIFTFACTOR_X, gb_finder->CurrentY() + SHIFTFACTOR_Y, 0);
		glScalef(0.1, 0.1, 1);
		gb_finder->Draw();
	}

	glutSwapBuffers();
}

// the space bar toggles making maze or pause
void keyFunc( unsigned char key, int x, int y ){
	switch (key) {
	case ' ':
		work = !work;
		break;
	case '-':
		if (timefactor < 50) timefactor += 5;	// work faster
		break;
	case '+':
		timefactor -= 5;	// work slower
		if( timefactor < 0) timefactor = 0;
		break;
	case 0x7f:	// initialzing all factors, (delete key)
		Over_view = false;
		ViewFactor = 20;
		ViewChange_x = 0;
		ViewChange_y = 0;
		timefactor = INIT_TIMEFACTOR;
		reviewpoint();
		display();
		break;
	}
}

void path_finding()
{
	static int oldTime;
	int currTime = timeGetTime();
	static PathFinder finder(::starting_x, ::starting_y);
	gb_finder = &finder;	// to use in other functions
	static int x = ::starting_x;
	static int y = ::starting_y;

	if(currTime - oldTime > timefactor)
		oldTime = currTime;
	else return;

	finder.UpdateStatus();
	if(finder.isMoving()) {
		return;
	}

	if(x == ::goal_x && y == ::goal_y) {	// if get the goal
		::state++;
		gb_finder->Set_getgoal();
		return;
	}

	cellXY(x, y).is_open = true;	// visit starting position
	if(finder.isStack_Empty() || finder.Stack_Top() < NOT_ANY_DIRECTION) {
		if(cellXY(x,  y).road[down] == true && y > 0 && cellXY(x, y-1).is_open == false) {
			finder.set_dest(down);
			y--;
			cellXY(x, y).is_open = true;
			finder.Stack_Push(down);
			return;
		}
		else finder.Stack_Push(NOTDOWN);
	}
	if(finder.Stack_Top() == NOTDOWN) {
		if(cellXY(x, y).road[left] == true && x > 0 && cellXY(x-1, y).is_open == false) {
			finder.set_dest(left);
			x--;
			cellXY(x, y).is_open = true;
			finder.Stack_Push(left);
			return;
		}
		else finder.Stack_Push(NOTLEFT);
	}
	if(finder.Stack_Top() == NOTLEFT) {
		if(cellXY(x, y).road[right] == true && x < ::width-1 && cellXY(x+1, y).is_open == false) {
			finder.set_dest(right);
			x++;
			cellXY(x, y).is_open = true;
			finder.Stack_Push(right);
			return;
		}
		else finder.Stack_Push(NOTRIGHT);
	}
	if(finder.Stack_Top() == NOTRIGHT) {
		if(cellXY(x, y).road[up] == true && y < ::height-1 && cellXY(x, y+1).is_open == false) {
			finder.set_dest(up);
			y++;
			cellXY(x, y).is_open = true;
			finder.Stack_Push(up);
			return;
		}
		else finder.Stack_Push(NOTUP);
	}

	int temp_dest;
	finder.Stack_Pop();
	finder.Stack_Pop();
	finder.Stack_Pop();
	finder.Stack_Pop();
	temp_dest = finder.Stack_Pop();
	if(temp_dest == down) y++, finder.set_dest(up);
	else if(temp_dest == left) x++, finder.set_dest(right);
	else if(temp_dest == right) x--, finder.set_dest(left);
	else if(temp_dest == up) y--, finder.set_dest(down);
}

void goal_ceremony()
{
	static int count = 0;
	static int oldTime = 0;
	int currTime = timeGetTime();

	if(currTime - oldTime < timefactor * 0.2) return;

	oldTime = currTime;
	count++;
	if( count > 300) state++;
	gb_finder->UpdateStatus();
}

void specialKeyFunc( int key, int x, int y ){
	switch (key) {
	case GLUT_KEY_RIGHT:
		if (ViewChange_x < width * 5) ViewChange_x += 5;	// scroll the maze right
		break;
	case GLUT_KEY_LEFT:
		if (ViewChange_x > width * -5) ViewChange_x -= 5;	// scroll the maze left
		break;
	case GLUT_KEY_DOWN:
		if (ViewChange_y > height * -5) ViewChange_y -= 5;	// scroll the maze down
		break;
	case GLUT_KEY_UP:
		if (ViewChange_y < height * 5) ViewChange_y += 5;	// scroll the maze up
		break;
	case GLUT_KEY_PAGE_DOWN:
		if (ViewFactor < ((width>height)? width * 5 : height * 5)) ViewFactor += 5;	// zoom in
		break;
	case GLUT_KEY_PAGE_UP:
		if (ViewFactor > 0) ViewFactor -= 5;	// zoom out
		break;
	case GLUT_KEY_INSERT:	// initializing viewing
		ViewFactor = 20;
		ViewChange_x = 0;
		ViewChange_y = 0;
		break;
	case GLUT_KEY_HOME:
		Over_view = true;	// set the whole maze can be seen
		break;
	case GLUT_KEY_END:
		Over_view = false;	// zoomed maze
		break;
	}

	reviewpoint();
	display();
}

void reviewpoint()
{
	if(gb_finder == NULL) return;	// if finder does not exist

	if( Over_view == true ){
		double move = (width-height)/2.0;
		if( width >= height ){
			view_Left = 0.0;
			view_Right = 20 + width*10;
			view_Bottom = 0.0 - move*10;
			view_Up = width*10 + 20 - move*10;
		}
		else{
			view_Left = 0.0 + move*10;
			view_Right = 20 + height*10 + move*10;
			view_Bottom = 0.0;
			view_Up = height*10 + 20;
		}
	}

	else{
		view_Left = gb_finder->CurrentX() - (ViewFactor+10)+ ViewChange_x;
		view_Right = gb_finder->CurrentX() + ViewFactor+ ViewChange_x;
		view_Bottom = gb_finder->CurrentY() - (ViewFactor+10)+ ViewChange_y;
		view_Up = gb_finder->CurrentY() + ViewFactor+ ViewChange_y;
	}
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluOrtho2D( view_Left, view_Right, view_Bottom, view_Up );

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}

void idle()
{
	if( work == false ) return;
	switch (state) {
	case 0:
		gen_maze();
		break;
	case 1:
		path_finding();
		reviewpoint();
		break;
	case 2:
		goal_ceremony();
		break;
	case 3:
		exit(0);
		break;
	}
	display();
}

int main( int argc, char ** argv ){
	using namespace std;

	srand( ( unsigned )time( NULL ) );

	/* input the size of the maze */
	while(1) {
		cout<<"please input the size of maze( width, 5 ~ 50 )"<<endl;
		cin>>width;
		if( width > 50 || width < 5 )	cout<<"out of range!"<<endl;
		else break;
	}
	while(1) {
		cout<<"please input the size of maze( height, 5 ~ 50 )"<<endl;
		cin>>height;
		if( height > 50 || height < 5 )	cout<<"out of range!"<<endl;
		else break;
	}

	/* help message */
	cout << endl;
	cout << "Space bar : start/stop working" << endl;
	cout << " + key    : increasing speed" << endl;
	cout << " - key    : decreasing speed" << endl;
	cout << "Arrow key : scroll the maze" << endl;
	cout << "Page Up   : Zoom in" << endl;
	cout << "Page Down : Zoom out" << endl;
	cout << "Home key  : Over view the maze" << endl;
	cout << "End key   : closed view the maze" << endl;
	cout << "Insert key: initialize zoom and scroll" << endl;
	cout << "Del key   : initialize all(zoom, scroll, speed)" << endl;
	cout << endl << "Check the newly created window!" << endl;

	cell = new Cell[width * height];

	/* choose background color */
	R = ( rand()%256 ) / 255.0;
	G = ( rand()%256 ) / 255.0;
	B = ( rand()%256 ) / 255.0;

	timefactor = INIT_TIMEFACTOR;
	ViewChange_x = 0;
	ViewChange_y = 0;
	ViewFactor = 20;

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("miro");
	glutReshapeFunc( reshape );
	glutDisplayFunc( display );
	glutIdleFunc( idle );
	glutSpecialFunc( specialKeyFunc );
	glutKeyboardFunc( keyFunc );

	glutMainLoop();

	return 0;
}
