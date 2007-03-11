/**
	\file "main/libhackt-wrap.cc"
	$Id: libhackt-wrap.cc,v 1.1 2007/03/11 21:16:51 fang Exp $
 */

#include "main/libhackt-wrap.h"
#include "util/libguile.h"
#include <iostream>
#include "Object/module.h"

namespace HAC {
//=============================================================================
// convention: all function names shall begin with 'wrap_'

namespace guile_wrap {
static
SCM
wrap_objdump(void);
}	// end namespace guile
namespace entity {

}	// end namespace entity
//=============================================================================
}	// end namespace HAC

//=============================================================================
// implementations

namespace HAC {
namespace guile_wrap {
#include "util/using_ostream.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// static global initialization
excl_ptr<module>	obj_module(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SCM
wrap_objdump(void) {
#define	FUNC_NAME "objdump"
	NEVER_NULL(obj_module);
	obj_module->dump(cout);
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}	// end namespace guile
}	// end namespace HAC

//=============================================================================

BEGIN_C_DECLS
using namespace HAC::guile_wrap;

/**
	\pre guile is already booted.
 */
void
libhackt_guile_init(void) {
	typedef	SCM (*scm_gsubr_type)();
	// ugh, function pointer reinterpret_cast...
	scm_c_define_gsubr("objdump", 0, 0, 0, wrap_objdump);
}
END_C_DECLS


