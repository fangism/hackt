/**
	\file "Object/expr/dlfunction_fwd.hh"
	Forward declarations of types. 
	$Id: dlfunction_fwd.hh,v 1.4 2010/10/23 23:49:00 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DLFUNCTION_FWD_H__
#define	__HAC_OBJECT_EXPR_DLFUNCTION_FWD_H__

// installed development headers should ideally not require any "config.h"
#include "util/memory/pointer_classes_fwd.hh"

namespace HAC {
namespace entity {
//=============================================================================
using util::memory::count_ptr;
using util::memory::never_ptr;
class const_param;
class const_param_expr_list;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	count_ptr<const_param>		meta_function_argument_type;
typedef	count_ptr<const const_param>	meta_function_const_argument_type;
typedef	count_ptr<const const_param>	meta_function_return_type;

// functions are evaluated after their arguments have been resolved to consts
typedef	const_param_expr_list		meta_function_argument_list_type;
typedef	meta_function_return_type
        (meta_function_type)(const meta_function_argument_list_type&);
// using pointer-class will guarantee that value is at least NULL-initialized
typedef	never_ptr<meta_function_type>	meta_function_ptr_type;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// CHP functions use the exact same

typedef	meta_function_argument_type		chp_function_argument_type;
typedef	meta_function_const_argument_type	chp_function_const_argument_type;
typedef	meta_function_return_type		chp_function_return_type;
typedef	meta_function_argument_list_type	chp_function_argument_list_type;

typedef meta_function_type			chp_dlfunction_type;
typedef meta_function_ptr_type			chp_dlfunction_ptr_type;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DLFUNCTION_FWD_H__

