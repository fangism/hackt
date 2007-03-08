/**
	\file "guile-logo/tortoise-wrap.cc"
	$Id: tortoise-wrap.cc,v 1.1 2007/03/08 23:07:28 fang Exp $
	Wrapper implementation.  
 */

#include "tortoise-wrap.h"
#include "tortoise.h"

namespace logo {
namespace guile {
//=============================================================================
SCM
wrap_tortoise_reset(void) {
#define FUNC_NAME "tortoise-reset"
	tortoise_reset();
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SCM
wrap_tortoise_pendown(void) {
#define FUNC_NAME "tortoise-pendown"
	tortoise_pendown();
	return SCM_UNSPECIFIED;
#undef	FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SCM
wrap_tortoise_penup(void) {
#define FUNC_NAME "tortoise-penup"
	tortoise_penup();
	return SCM_UNSPECIFIED;
#undef FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SCM
wrap_tortoise_turn(SCM s_0) {
#define FUNC_NAME "tortoise-turn"
	const int arg1 = scm_num2int(s_0, SCM_ARG1, FUNC_NAME);
	tortoise_turn(arg1);
	return SCM_UNSPECIFIED;
#undef FUNC_NAME
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SCM
wrap_tortoise_move(SCM s_0) {
#define FUNC_NAME "tortoise-move"
	const int arg1 = scm_num2int(s_0, SCM_ARG1, FUNC_NAME);
	tortoise_move(arg1);
	return SCM_UNSPECIFIED;
#undef FUNC_NAME
}
//=============================================================================
}	// end namespace guile
}	// end namespace logo

//=============================================================================
extern "C" {
using namespace logo::guile;
/**
	Initialization should be a C-linkage function to avoid
	having to mangle name.  
 */
void
logo_guile_init(void) {
	scm_c_define_gsubr("tortoise-reset", 0, 0, 0, wrap_tortoise_reset);
	scm_c_define_gsubr("tortoise-pendown", 0, 0, 0, wrap_tortoise_pendown);
	scm_c_define_gsubr("tortoise-penup", 0, 0, 0, wrap_tortoise_penup);
	scm_c_define_gsubr("tortoise-turn", 1, 0, 0, wrap_tortoise_turn);
	scm_c_define_gsubr("tortoise-move", 1, 0, 0, wrap_tortoise_move);
}

}	// extern "C"

