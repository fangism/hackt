/**
	\file "Object/expr/dlfunction.h"
	Header that defines API for linking chpsim to external,
	dynamically loaded functions.  
	This header should be installed.  
	Since these symbols are bound in the executable 
	(or its shared libraries), the executable needs to be linked 
	-export-dynamic.  
	$Id: dlfunction.h,v 1.5 2010/09/21 00:18:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DLFUNCTION_H__
#define	__HAC_OBJECT_EXPR_DLFUNCTION_H__

#include <iosfwd>
#include <string>
// installed development headers should ideally not require any "config.h"
#include "Object/expr/dlfunction_fwd.h"
#include "Object/expr/types.h"
// #include "util/attributes.h"		// bah!

namespace HAC {
namespace entity {
/**
	This binds names to function symbols from dlopened modules.  
	Loaders should call this in their init() routines, 
	after dependent modules have been loaded.
	Recommend using the automatic registration class interface instead.
 */
extern
int
register_chpsim_function(const std::string&, const chp_dlfunction_ptr_type);

// Q: do we ever want to un-register functions?

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper class for automatically registering function
	upon dlopening of a module.  
 */
class chp_function_registrar {
public:
	/**
		\throw exception on failure.
	 */
	chp_function_registrar(const std::string&, 
		const chp_dlfunction_ptr_type);

	chp_function_registrar(const std::string&, 
		chp_dlfunction_type* const);

	~chp_function_registrar();

} /* __ATTRIBUTE_UNUSED__ */ ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the following functions are only needed for internal compiling
// not needed for the exported API
/**
	The lookup counterpart.  
 */
extern
chp_dlfunction_ptr_type
lookup_chpsim_function(const std::string&);

extern
void
list_chpsim_functions(std::ostream&);

extern
bool
ack_loaded_functions;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DLFUNCTION_H__

