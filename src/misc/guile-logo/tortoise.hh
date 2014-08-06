/**
	\file "misc/guile-logo/tortoise.h"
	$Id: tortoise.h,v 1.1 2007/01/27 05:50:59 fang Exp $
	Ripped from tutorial:
	http://www.gnu.org/software/guile/docs/guile-tut/tortoise1.html
 */

#ifndef	__MISC_GUILE_LOGO_TORTOISE_H__
#define	__MISC_GUILE_LOGO_TORTOISE_H__

/* #include <unistd.h> */
#if 0
#include <X11/Xlib.h>
#endif
#define WINDOW_SIZE 500
#define DEGREES_TO_RADIANS  (3.1415926535897932384626433832795029L/180.0)

/**
	Namespace of logo functions and variables.  
 */
namespace logo {
#if 0
// X11 objects
extern Display *theDisplay;
extern Window theWindow;
extern Screen *theScreen;
extern GC theGC;
#endif

#if 0
extern double currentX;
extern double currentY;
extern double currentDirection;
extern int penDown;
#endif

extern void tortoise_init_window(void);
extern void tortoise_reset(void);
extern void tortoise_pendown(void);
extern void tortoise_penup(void);
extern void tortoise_turn(const int degrees);
extern void tortoise_move(const int steps);

}	// end namespace logo

#endif	// __MISC_GUILE_LOGO_TORTOISE_H__

