/**
	\file "Object/expr/meta_func_lib.h"
	Meta-functions can be dynamically added.
	This header should be installed (eventually).  
	$Id: meta_func_lib.h,v 1.1.2.1 2010/09/15 00:57:54 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_FUNC_LIB_H__
#define	__HAC_OBJECT_EXPR_META_FUNC_LIB_H__

#include <iosfwd>
#include <string>
#include "Object/expr/dlfunction_fwd.h"
	// eventually reduce this include to forward declarations and typedefs
#include "Object/expr/types.h"

/**
	Until we support adding functionality through plug-in modules, 
	we privatize this interface.
 */
#define	LOADABLE_META_FUNCTIONS			0

namespace HAC {
namespace entity {
#if LOADABLE_META_FUNCTIONS
/**
	This binds names to function symbols from dlopened modules.  
	Loaders should call this in their init() routines, 
	after dependent modules have been loaded.
	Recommend using the automatic registration class interface instead.
 */
extern
int
register_meta_function(const std::string&, const meta_function_ptr_type);
#endif

// Q: do we ever want to un-register functions?

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper class for automatically registering function
	upon dlopening of a module.  
 */
class meta_function_registrar {
public:
	/**
		\throw exception on failure.
	 */
	meta_function_registrar(const std::string&, 
		const meta_function_ptr_type);

	meta_function_registrar(const std::string&, 
		meta_function_type* const);

	~meta_function_registrar();

} /* __ATTRIBUTE_UNUSED__ */ ;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the following functions are only needed for internal compiling
// not needed for the exported API
/**
	The lookup counterpart.  
 */
extern
meta_function_ptr_type
lookup_meta_function(const std::string&);

extern
void
list_meta_functions(std::ostream&);

#if 0
extern
bool
ack_loaded_functions;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_META_FUNC_LIB_H__

