/**
	\file "guile-logo/tortoise-wrap.h"
	$Id: tortoise-wrap.h,v 1.2 2007/03/11 21:16:53 fang Exp $
	guile-scheme interface prototypes.  
	Keep this file synchronized with interface from "guile-logo/tortoise.h"
 */

#ifndef	__GUILE_LOGO_TORTOISE_WRAP_H__
#define	__GUILE_LOGO_TORTOISE_WRAP_H__

#include "util/libguile.hh"

namespace logo {
namespace guile {

extern SCM
wrap_tortoise_reset(void);

extern SCM
wrap_tortoise_pendown(void);

extern SCM
wrap_tortoise_penup(void);

extern SCM
wrap_tortoise_turn(SCM);

extern SCM
wrap_tortoise_move(SCM);

}	// end namespace guile
}	// end namespace logo

extern "C" {
extern void
logo_guile_init(void);
}

#endif	// __GUILE_LOGO_TORTOISE_WRAP_H__

