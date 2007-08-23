/**
	\file "Object/expr/dlfunction_fwd.h"
	Forward declarations of types. 
	$Id: dlfunction_fwd.h,v 1.1.2.1 2007/08/23 06:57:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_DLFUNCTION_FWD_H__
#define	__HAC_OBJECT_EXPR_DLFUNCTION_FWD_H__

// installed development headers should ideally not require any "config.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
//=============================================================================
using util::memory::count_ptr;
class const_param;
class const_param_expr_list;

typedef	count_ptr<const_param>		chp_function_argument_type;
typedef	count_ptr<const const_param>	chp_function_const_argument_type;
typedef	count_ptr<const const_param>	chp_function_return_type;
typedef	const_param_expr_list		chp_function_argument_list_type;

typedef
chp_function_return_type
(chp_dlfunction_type) (const chp_function_argument_list_type&);

typedef	chp_dlfunction_type*		chp_dlfunction_ptr_type;

class chp_function_registrar;

template <class T>
struct chp_call_traits;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_DLFUNCTION_FWD_H__

