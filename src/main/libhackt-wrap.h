/**
	\file "main/libhackt-wrap.h"
	$Id: libhackt-wrap.h,v 1.2 2007/03/14 04:06:23 fang Exp $
 */

#ifndef	__HAC_MAIN_LIBACKT_WRAP_H__
#define	__HAC_MAIN_LIBACKT_WRAP_H__

#include "util/c_decl.h"
#include "util/memory/excl_ptr.h"

#if 0
#include "util/libguile.h"

namespace HAC {
//=============================================================================
// convention: all function names shall begin with 'wrap_'
namespace guile {
extern
void
wrap_objdump(void);
}	// end namespace guile

namespace entity {

}	// end namespace entity
//=============================================================================
}	// end namespace HAC
#else
// what if we just expose the interface loader only?
#endif

//=============================================================================
// global data to be made accessible to guile interpreter

namespace HAC {
namespace entity {
class module;
}	// end namespace entity

/// namespace for guile-wrappers and interfaces
namespace guile_wrap {
using util::memory::excl_ptr;
using entity::module;
/**
	Top-level object module to be loaded before passing control
	over to guile/scheme interpreter.
	Shouldn't be const, because we may need to allocate.
 */
extern	excl_ptr<module>	obj_module;
}	// end namespace guile
}	// end namespace HAC


//=============================================================================
BEGIN_C_DECLS
extern
void
libhackt_guile_init(void);
END_C_DECLS

#endif	// __HAC_MAIN_LIBACKT_WRAP_H__

