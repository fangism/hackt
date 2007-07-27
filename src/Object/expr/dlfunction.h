/**
	\file "Object/expr/dlfunction.h"
	Header that defines API for linking chpsim to external,
	dynamically loaded functions.  
	This header should be installed.  
	Since these symbols are bound in the executable 
	(or its shared libraries), the executable needs to be linked 
	-export-dynamic.  
	$Id: dlfunction.h,v 1.1.2.4 2007/07/27 05:33:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DLFUNCTION_H__
#define	__HAC_OBJECT_EXPR_DLFUNCTION_H__

#include <iosfwd>
#include <string>
// installed development headers should ideally not require any "config.h"
#include "util/memory/pointer_classes_fwd.h"
#include "Object/expr/types.h"
// #include "util/attributes.h"		// bah!

namespace HAC {
namespace entity {
//=============================================================================
using util::memory::count_ptr;
class const_param;
class const_param_expr_list;

typedef	count_ptr<const_param>		chp_function_argument_type;
typedef	count_ptr<const const_param>	chp_function_const_argument_type;
typedef	count_ptr<const_param>		chp_function_return_type;

typedef
chp_function_return_type
(chp_dlfunction_type) (const const_param_expr_list&);

typedef	chp_dlfunction_type*		chp_dlfunction_ptr_type;

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

	~chp_function_registrar();

} /* __ATTRIBUTE_UNUSED__ */ ;

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

/**
	intentionally does not use template argument deduction
	Also only explicitly instantiated/specialized for select types.  
	Primary template is undefined.
 */
template <typename V>
V
extract_chp_value(const chp_function_const_argument_type&);

// forward declare specializations
template <>
int_value_type
extract_chp_value<int_value_type>(const chp_function_const_argument_type&);

template <>
bool_value_type
extract_chp_value<bool_value_type>(const chp_function_const_argument_type&);

template <>
real_value_type
extract_chp_value<real_value_type>(const chp_function_const_argument_type&);

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

extern
void
list_chpsim_functions(std::ostream&);

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DLFUNCTION_H__

