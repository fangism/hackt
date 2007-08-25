/**
	\file "Object/expr/dlfunction.h"
	Header that defines API for linking chpsim to external,
	dynamically loaded functions.  
	This header should be installed.  
	Since these symbols are bound in the executable 
	(or its shared libraries), the executable needs to be linked 
	-export-dynamic.  
	$Id: dlfunction.h,v 1.3.2.3 2007/08/25 08:12:14 fang Exp $
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
// call_traits

/**
	Template policy for passing/returning by value or reference.
 */
template <class T>
struct chp_call_traits;

template <>
struct chp_call_traits<int_value_type> {
	typedef	int_value_type		type;
	typedef	int_value_type		return_type;
	typedef	const int_value_type	argument_type;
};

template <>
struct chp_call_traits<bool_value_type> {
	typedef	bool_value_type		type;
	typedef	bool_value_type		return_type;
	typedef	const bool_value_type	argument_type;
};

/**
	Consider changing to reference when upgrading to doubles.  
 */
template <>
struct chp_call_traits<real_value_type> {
	typedef	real_value_type		type;
	typedef	real_value_type		return_type;
	typedef	const real_value_type	argument_type;
};

template <>
struct chp_call_traits<string_value_type> {
	typedef	string_value_type		type;
//	typedef	const string_value_type&	return_type;
// should be fast, std::string is shallow ref-counted copy-on-write
	typedef	string_value_type		return_type;
	typedef	const string_value_type&	argument_type;
};

template <>
struct chp_call_traits<const string_value_type&> {
	typedef	string_value_type		type;
//	typedef	const string_value_type&	return_type;
// should be fast, std::string is shallow ref-counted copy-on-write
	typedef	string_value_type		return_type;
	typedef	const string_value_type&	argument_type;
};

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

extern
string_value_type
extract_string(const const_param&);

/**
	intentionally does not use template argument deduction
	Also only explicitly instantiated/specialized for select types.  
	Primary template is undefined.
 */
template <typename V>
typename chp_call_traits<V>::return_type
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

template <>
string_value_type
extract_chp_value<string_value_type>(const chp_function_const_argument_type&);

template <>
inline
string_value_type
extract_chp_value<const string_value_type&>(
		const chp_function_const_argument_type& a) {
	return extract_chp_value<string_value_type>(a);
}

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

extern
chp_function_return_type
make_chp_value(const string_value_type&);

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

