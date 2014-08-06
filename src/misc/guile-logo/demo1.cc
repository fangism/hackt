/**
	\file "misc/guile-logo/tortoise.cc"
	$Id: demo1.cc,v 1.1 2007/01/27 05:50:57 fang Exp $
	Ripped from tutorial:
	http://www.gnu.org/software/guile/docs/guile-tut/tortoise1.html
 */
/* Non-guiled version of the program */
#include <unistd.h>
#include "tortoise.hh"

using namespace logo;

int
main(int argc, char *argv[]) {
#if 0
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
#endif
    tortoise_init_window();
    /* more stuff to come here . . */
    tortoise_reset();
    {
        int ii;
        tortoise_pendown();
        for (ii=0; ii<4; ++ii) {
            tortoise_move(100);
            tortoise_turn(90);
        }
        /* sleep for a bit so the window stays visible */
        sleep(5); 
    }
    return 0;
}

