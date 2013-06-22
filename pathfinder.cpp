#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdlib>

#include "pathfinder.h"
#include "miro.h"

extern double R, G, B;		// the color of background
extern int width, height;	// the size of maze

void PathFinder::set_dest( int new_dest ){
	this->init_dest = Dest;
	this->Dest = new_dest;
	rolling_status = 0;
	ismoving =true;
}

PathFinder::PathFinder(int x_position, int y_position, double HEIGHT, double WIDTH)
{
	old_x = current_x = 10.0 + 10.0 * x_position;
	old_y = current_y = 10.0 + 10.0 * y_position;
	height = HEIGHT;
	width = WIDTH;
	recursion_stack = new int[::width * ::height * 4];	// size of worst case
	stack_top = -1;

	/* initialzing status factor */
	ismoving = false;
	walk_status = 0;
	eye_status = 0;
	rolling_status = 0;
	goal_ceremony_status = 0;
	degree_7 = sin(7 * atan(-1) / 180);	// sin( 7 * PI / 180)

	lists();
	init_dest = Dest = right;
}

void PathFinder::lists(){
	glNewList( Arm, GL_COMPILE );
		glBegin( GL_POLYGON );
			glEdgeFlag( GL_TRUE );
			glVertex2f( 0, 0 );
			glVertex2f( 15, -10 );
			glVertex2f( 25, -25 );
			//glVertex2f( 0, 0 );
		glEnd();
	glEndList();

	glNewList( Leg, GL_COMPILE );
		glBegin( GL_POLYGON );
			glEdgeFlag( GL_TRUE );
			glVertex2f( 0, 0 );
			glVertex2f( 7, -28.5 );
			glVertex2f( 0, -28.5 );
			//glVertex2f( 0, 0 );
		glEnd();
	glEndList();

	glNewList( Eye, GL_COMPILE );
		glColor3f( 1, 1, 1 );
		glBegin( GL_POLYGON );
			glEdgeFlag( GL_TRUE );
			glVertex2f( 0, 0 );
			glVertex2f( 10, -3 );
			glVertex2f( 8, -10 );
			glVertex2f( 2, -9 );
			//glVertex2f( 0, 0 );
		glEnd();

		glBegin( GL_POLYGON );
			glColor3f( 0, 0, 0 );
			glVertex2f( 6, -1.8 );
			glVertex2f( 8, -2.4 );
			glVertex2f( 8, -8 );
			glVertex2f( 6, -8 );
			//glVertex2f( 8, -2.4 );
		glEnd();
	glEndList();

	glNewList( Body, GL_COMPILE );
		glBegin( GL_POLYGON );
			glEdgeFlag( GL_TRUE );
			glVertex2f( 0, 0 );
			glVertex2f( 40, 0  );
			glVertex2f( 30, 40 );
			glVertex2f( 10, 40 );
			//glVertex2f( 0, 0 );
		glEnd();
	glEndList();

}

void PathFinder::Move()
{
	double movingfactor = 28.5 * fabs(sin(degree_7 * walk_status) - sin(degree_7 * (walk_status - 1)));
	// length of leg * abs_value( sin(7_deg * walk_status) - sin(7_deg * (walk_status-1)))
	// movement of one frame of the animation

	if( rolling_status == ROLL_FACT ){
		switch(Dest) {
		case up:
			current_y += movingfactor;//( walk_status >= 0 )? walk_status/3.0 : -walk_status/3.0;
			break;
		case down:
			current_y -= movingfactor;//( walk_status >= 0 )? walk_status/3.0 : -walk_status/3.0;
			break;
		case left:
			current_x -= movingfactor;//( walk_status >= 0 )? walk_status/3.0 : -walk_status/3.0;
			break;
		case right:
			current_x += movingfactor;//( walk_status >= 0 )? walk_status/3.0 : -walk_status/3.0;
			break;
		}
	}

	if(abs(old_x - current_x) >= 10.0) {
		current_x = old_x + ((Dest == right) ? 10 : -10);
		old_x = current_x;
		ismoving = false;
	}
	else if(abs(old_y - current_y) >= 10.0) {
		current_y = old_y + ((Dest == up) ? 10 : -10);
		old_y = current_y;
		ismoving = false;
	}

	if(rolling_status == ROLL_FACT) {	// if it is rotating then do not move the leg, arm and eye
		walk_status++;
		if( walk_status > 5 )
			walk_status = -4;
		eye_status++;
		if( eye_status >= 30 )
			eye_status = 0;
	}
}

void PathFinder::Draw()
{
	glLoadIdentity();
	glTranslatef(current_x + SHIFTFACTOR_X, current_y + SHIFTFACTOR_Y, 0);
	glScalef(height, width, 1);

	glColor3f( 1.0-R, 1.0-G, 1.0-B );	// color of body
	glTranslatef( 30, 50, 0 );

	if( init_dest == left ) glTranslatef(20,15,0), glRotatef( 180, 0, 0, 1 ),glTranslatef(-20, -15, 0);
	if( init_dest == up )   glTranslatef(20,15,0), glRotatef( 90, 0, 0, 1 ), glTranslatef(-20, -15, 0);
	if( init_dest == down ) glTranslatef(20,15,0), glRotatef( -90, 0, 0, 1 ), glTranslatef(-20, -15, 0);

	if( rolling_status < ROLL_FACT ){
		rolling_status++;

		if( init_dest == Dest ) rolling_status = ROLL_FACT;	//if it is not need to roll
		else if( init_dest == left ){
			if( Dest == right )
				glTranslatef(20,15,0), glRotatef( 180/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == up )
				glTranslatef(20,15,0), glRotatef( -90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == down )
				glTranslatef(20,15,0), glRotatef( 90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
		}
		else if( init_dest == right ){
			if( Dest == left )
				glTranslatef(20,15,0), glRotatef( 180/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == up )
				glTranslatef(20,15,0), glRotatef( 90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == down )
				glTranslatef(20,15,0), glRotatef( -90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
		}
		else if( init_dest == up ){
			if( Dest == down )
				glTranslatef(20,15,0), glRotatef( 180/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == left )
				glTranslatef(20,15,0), glRotatef( 90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == right )
				glTranslatef(20,15,0), glRotatef( -90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
		}
		else{	// initdest == down
			if( Dest == left )
				glTranslatef(20,15,0), glRotatef( -90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == up )
				glTranslatef(20,15,0), glRotatef( 180/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
			else if( Dest == right )
				glTranslatef(20,15,0), glRotatef( 90/ROLL_FACT*rolling_status, 0, 0, 1 ), glTranslatef(-20, -15, 0);
		}
		if( rolling_status == ROLL_FACT )
			init_dest = Dest;
	}
	glCallList( Body );	// draw body

	glPushMatrix();

	/* draw eye */
	glTranslatef( 23, 30, 0 );
	if( eye_status >= 24 ) glScalef( 1, ( 30 - eye_status -1 )/6.0, 1);
	glCallList( Eye );

	/* draw arms */
	glPopMatrix();
	glPushMatrix();
	glColor3f( 0.0, 0.0, 0.0 );
	glTranslatef( 35, 20, 0 );
	if( get_goal == true) glRotatef( 5 * goal_ceremony_status, 0, 0, 1);
	else glRotatef(abs(walk_status * 5), 0, 0, 1);
		//if( walk_status >= 0 ) glRotatef( 5*walk_status, 0, 0, 1 );
		//else glRotatef( walk_status * -5 , 0, 0, 1 );
	glCallList( Arm );

	glPopMatrix();
	glPushMatrix();
	glTranslatef( 5, 20, 0 );
	glRotatef( 180, 0 ,1, 0 );
	if( get_goal == true) glRotatef( -5 * goal_ceremony_status , 0, 0, 1);
	else glRotatef(abs(walk_status * 5), 0, 0, 1);
		//if( walk_status >= 0 ) glRotatef( 5*walk_status, 0, 0, 1 );
		//else glRotatef( walk_status * -5, 0, 0, 1 );
	glCallList( Arm );

	/* draw legs */
	glPopMatrix();
	glPushMatrix();
	glTranslatef( 32, 0, 0 );
	if( get_goal == false) glRotatef( abs(7 * walk_status), 0, 0, 1 );
		//if( walk_status >= 0 ) glRotatef( 7 * walk_status, 0, 0, 1 );
		//else glRotatef( walk_status * -7 , 0, 0, 1 );
	glCallList( Leg );

	glPopMatrix();
	glPushMatrix();
	glTranslatef( 8, 0, 0 );
	glRotatef( 180, 0, 1, 0 );
	if( get_goal == false) glRotatef( abs(7 * walk_status), 0, 0, 1 );
		//if( walk_status >= 0 ) glRotatef( 7 * walk_status, 0, 0, 1 );
		//else glRotatef( walk_status * -7 , 0, 0, 1 );
	glCallList( Leg );
	glPopMatrix();

	glLoadIdentity();
}

void PathFinder::UpdateStatus() {
	if(get_goal == true) goal_ceremony_status++;
}
