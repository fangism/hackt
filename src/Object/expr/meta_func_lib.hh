/**
	\file "Object/expr/meta_func_lib.hh"
	Meta-functions can be dynamically added.
	This header should be installed (eventually).  
	$Id: meta_func_lib.hh,v 1.2 2010/09/21 00:18:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_META_FUNC_LIB_H__
#define	__HAC_OBJECT_EXPR_META_FUNC_LIB_H__

#include <iosfwd>
#include <string>
#include <utility>			// for std::pair
#include "Object/expr/dlfunction_fwd.hh"
	// eventually reduce this include to forward declarations and typedefs
#include "Object/expr/types.hh"

/**
	Until we support adding functionality through plug-in modules, 
	we privatize this interface.
 */
#define	LOADABLE_META_FUNCTIONS			0

namespace HAC {
namespace entity {
#define	REGISTER_META_FUNCTION_PROTO					\
int register_meta_function(const std::string&, 				\
	const meta_function_ptr_type, const char)

#if LOADABLE_META_FUNCTIONS
/**
	This binds names to function symbols from dlopened modules.  
	Loaders should call this in their init() routines, 
	after dependent modules have been loaded.
	Recommend using the automatic registration class interface instead.
 */
extern
REGISTER_META_FUNCTION_PROTO;
#endif

// Q: do we ever want to un-register functions?
typedef	meta_function_ptr_type		meta_mapped_func_ptr_type;

/**
	second: char is actually a meta_type_tag_enum from class_traits.
 */
typedef std::pair<meta_mapped_func_ptr_type, char>
					mapped_func_entry_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the following functions are only needed for internal compiling
// not needed for the exported API
/**
	The lookup counterpart.  
 */
extern
const mapped_func_entry_type&
lookup_meta_function(const std::string&);

extern
char
lookup_meta_function_return_tag(const std::string&);

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

