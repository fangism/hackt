/**
	\file "Object/expr/dlfunction.h"
	Header that defines API for linking chpsim to external,
	dynamically loaded functions.  
	This header should be installed.  
	Since thes symbols are bound in the executable 
	(or its shared libraries), the executable needs to be linked 
	-export-dynamic.  
	$Id: dlfunction.h,v 1.1.2.1 2007/07/23 22:17:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DLFUNCTION_H__
#define	__HAC_OBJECT_EXPR_DLFUNCTION_H__

#include <string>
// installed development headers should ideally not require any "config.h"
#include "util/memory/pointer_classes_fwd.h"
#include "Object/expr/types.h"
// #include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
//=============================================================================
using util::memory::count_ptr;
class const_param;
class const_param_expr_list;

typedef	count_ptr<const_param>		chp_function_argument_type;
typedef	count_ptr<const_param>		chp_function_return_type;

typedef
chp_function_return_type
(chp_dlfunction_type) (const const_param_expr_list&);

typedef	chp_dlfunction_type*		chp_dlfunction_ptr_type;

/**
	This binds names to function symbols from dlopened modules.  
	Loaders should call this in their init() routines, 
	after dependent modules have been loaded.
 */
extern
int
register_chpsim_function(const std::string&, const chp_dlfunction_ptr_type);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// wrappers to extract native type from dynamic HAC type
// each of these throw exceptions in the event of type-check failure

extern
int_value_type
extract_int(const const_param&);

extern
bool_value_type
extract_bool(const const_param&);

// currently float, see "Object/expr/types.h"
extern
real_value_type
extract_real(const const_param&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// these require count_ptr and const_param to be complete types...
extern
chp_function_return_type
make_chp_value(const int_value_type);

extern
chp_function_return_type
make_chp_value(const bool_value_type);

extern
chp_function_return_type
make_chp_value(const real_value_type);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the following functions are only needed for internal compiling
// not needed for the exported API
/**
	The lookup counterpart.  
 */
extern
chp_dlfunction_ptr_type
lookup_chpsim_function(const std::string&);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DLFUNCTION_H__

