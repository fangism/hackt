/**
	"common/ltdl-wrap.h"
	Wrapped interfaces for libltdl.  
	$Id: ltdl-wrap.h,v 1.2 2007/07/31 23:23:28 fang Exp $
 */

#ifndef	__HAC_COMMON_LTDL_WRAP_H__
#define	__HAC_COMMON_LTDL_WRAP_H__

// relative path given to avoid adding another CPPFLAG
#include "../libltdl/ltdl.h"	// needed for the types
// do we ever want to fallback to the system's installed ltdl?
#include "util/attributes.h"
#include "util/type_traits.h"
#include <string>

namespace HAC {
//=============================================================================
/**
	Class whose sole responsibility is to balance dlinit with dlexit,
	using constructor/destructor duality, thereby guaranteeing
	exception safety and non-leaking.
 */
class ltdl_token {
public:
	ltdl_token();
	~ltdl_token();

private:
	ltdl_token(const ltdl_token&);

	ltdl_token&
	operator = (const ltdl_token&);

	// also stack-allocate only...
} __ATTRIBUTE_UNUSED__ ;	// end class ltdl_token

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// assert that lt_dlhandle is a pointer
typedef	util::remove_pointer<lt_dlhandle>::type		lt_dlhandle_struct;
// typedef	const lt_dlhandle_struct*		lt_dlhandle_const;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Type-punning union because C++ forbids reinterpret_cast-ing
	between function and non-function pointers.  
	Reinterpret-cast-ing between function types is, however, allowed.  
 */
typedef	union {
	lt_ptr				nonfunc_ptr;
	void				(*func_ptr) (void);
} lt_dlsym_union;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper policy to automatically dlclose modules 
	as a destructor action.  
	Pass this along with pointer class policies.  
 */
struct ltdl_module_policy {
	void
	operator () (const lt_dlhandle);
};

/***
stupid question: does a module need to remain open to be able to 
use its symbols?
i.e. do we need to pass reference counted handles along with 
resolved symbols?
***/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This also retains a list of dlopened module handles to search through.  
	The opened module handle is prepended to the global search list.  
 */
extern
lt_dlhandle
ltdl_open_prepend(const std::string&);

/**
	The opened module handle is appended (back) to the global search list.  
 */
extern
lt_dlhandle
ltdl_open_append(const std::string&);

/**
	Search through list of modules until symbol is found.  
	Should we also return the module handle that owns the symbol?
 */
extern
lt_dlsym_union
ltdl_find_sym(const std::string&);

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_COMMON_LTDL_WRAP_H__

