/**
	\file "sim/chpsim/chpsim_dlfunction.h"
	This is the primary header to include for linking chpsim
	to dlopened libraries.  
	Try not to include other headers explicitly.  
	$Id: chpsim_dlfunction.h,v 1.5 2010/09/21 00:18:40 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__
#define	__HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

#include "util/cppcat.h"	// for uniquify
#include "Object/expr/dlfunction.h"
#include "Object/expr/meta_call_traits.h"
#include "Object/expr/types.h"

namespace HAC {
namespace entity {

//=============================================================================
#define	USING_CHPSIM_DLFUNCTION_PROLOGUE				\
using HAC::entity::chp_function_return_type;				\
using HAC::entity::chp_function_registrar;				\
using HAC::entity::chp_function_argument_list_type;			\
using HAC::entity::int_value_type;					\
using HAC::entity::bool_value_type;					\
using HAC::entity::real_value_type;					\
using HAC::entity::string_value_type;					\
using HAC::entity::const_param_expr_list;				\
using HAC::entity::extract_meta_value;					\
using HAC::entity::make_meta_value;

//=============================================================================
/**
	Declare an object that auto-loads the named function on construction.
	The function name is retained, not wrapped.  
	Part of public interface for 'advanced' users.
 */
#define	REGISTER_DLFUNCTION_RAW(key, fname)				\
static const chp_function_registrar					\
UNIQUIFY(fname ## _receipt_) (key, fname);

/**
	Name transformation, not for general use.  
 */
#define	WRAP_DLFUNCTION_NAME(fname)	UNIQUIFY(wrapped_ ## fname ## _)

/**
	Declare an object that auto-loads the named function on construction.
	Not intended for general use.  
 */
#define	REGISTER_DLFUNCTION(key, fname)					\
static const chp_function_registrar					\
UNIQUIFY(fname ## _receipt_) (key, WRAP_DLFUNCTION_NAME(fname));

/**
	Intended for general use.  
	\param key is the string for the function name.
	\param fname is the function in the local module to register.
 */
#define	CHP_DLFUNCTION_LOAD_DEFAULT(key, fname)				\
static									\
chp_function_return_type						\
WRAP_DLFUNCTION_NAME(fname) (const chp_function_argument_list_type& a) { \
	return auto_wrap_dlfunction(fname, a);				\
}									\
REGISTER_DLFUNCTION(key, fname)

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_CHPSIM_DLFUNCTION_H__

