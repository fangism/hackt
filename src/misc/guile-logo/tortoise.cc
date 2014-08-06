/**
	\file "misc/guile-logo/tortoise.cc"
	$Id: tortoise.cc,v 1.1 2007/01/27 05:50:59 fang Exp $
	Ripped from tutorial:
	http://www.gnu.org/software/guile/docs/guile-tut/tortoise1.html
 */
/* Non-guiled version of the program */
#include <cmath>
#include <unistd.h>
#include "tortoise.hh"
#include <X11/Xlib.h>

namespace logo {
//-----------------------------------------------------------------------------
// global variables and objects
static
Display *theDisplay = NULL;

static
Window theWindow;

static
Screen *theScreen = NULL;

static
GC theGC;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
double currentX;

static
double currentY;

static
double currentDirection;

static
int penDown;

//-----------------------------------------------------------------------------
/**
	Must call this first to startup a window.  
 */
void
tortoise_init_window(void) {
    theDisplay = XOpenDisplay(NULL);
    XSynchronize(theDisplay, True);
    theScreen = DefaultScreenOfDisplay(theDisplay);
    theWindow = XCreateSimpleWindow(theDisplay, RootWindowOfScreen(theScreen), 
                                    0, 0, 
                                    WINDOW_SIZE, WINDOW_SIZE, 0, 
                                    BlackPixelOfScreen(theScreen), 
                                    WhitePixelOfScreen(theScreen));
    theGC = XCreateGC(theDisplay, theWindow, 0L, NULL);
    XSetForeground(theDisplay, theGC, BlackPixelOfScreen(theScreen));
    XMapWindow(theDisplay,theWindow);  
    /* more stuff to come here . . */
    tortoise_reset();
}

//-----------------------------------------------------------------------------
void
tortoise_reset(void) {
    currentX = currentY = WINDOW_SIZE/2;
    currentDirection = 0.0;
    penDown = 1;
}

//-----------------------------------------------------------------------------
void
tortoise_pendown(void) {
    penDown = 1;
}

//-----------------------------------------------------------------------------
void
tortoise_penup(void) {
    penDown = 0;
}

//-----------------------------------------------------------------------------
void
tortoise_turn(const int degrees) {
    currentDirection += double(degrees);
}

//-----------------------------------------------------------------------------
void
tortoise_move(const int steps) {
    double newX, newY;
    /* first work out the new endpoint */
    newX = currentX + sin(currentDirection*DEGREES_TO_RADIANS)*double(steps);
    newY = currentY - cos(currentDirection*DEGREES_TO_RADIANS)*double(steps);
    /* if the pen is down, draw a line */
    if (penDown) {
	XDrawLine(theDisplay, theWindow, theGC, 
		  int(currentX), int(currentY), int(newX), int(newY));
    }
    /* in either case, move the tortoise */
    currentX = newX;
    currentY = newY;
}

//-----------------------------------------------------------------------------
}	// end namespace logo





