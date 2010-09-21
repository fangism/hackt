/**
	\file "Object/expr/meta_call_traits.cc"
	stolen from "Object/expr/dlfunction.cc"
	$Id: meta_call_traits.cc,v 1.2 2010/09/21 00:18:15 fang Exp $
 */

#define	ENABLE_STACKTRACE	0

#include <iostream>
#include <typeinfo>		// for std::bad_cast
#include "Object/expr/meta_call_traits.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pstring_const.h"
#include "util/macros.h"
#include "util/memory/count_ptr.h"
// #include "util/memory/excl_ptr.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using util::memory::never_ptr;
using std::string;
#include "util/using_ostream.h"

//=============================================================================
/**
	\throw a bad_cast on type-check failure.
 */
pint_value_type
extract_pint(const const_param& p) {
try {
	return IS_A(const pint_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting int, but got ") << endl;
	throw;		// re-throw
}
}

pbool_value_type
extract_pbool(const const_param& p) {
try {
	return IS_A(const pbool_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting bool, but got ") << endl;
	throw;		// re-throw
}
}

preal_value_type
extract_preal(const const_param& p) {
try {
	return IS_A(const preal_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting real, but got ") << endl;
	throw;		// re-throw
}
}

pstring_value_type
extract_pstring(const const_param& p) {
try {
	return IS_A(const pstring_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting string, but got ") << endl;
	throw;		// re-throw
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
pint_value_type
extract_meta_value<pint_value_type>(const meta_function_const_argument_type& v) {
	if (v)
		return extract_pint(*v);
	else {
		cerr << "Error extracting pint from NULL argument." << endl;
		THROW_EXIT;
	}
}

template <>
pbool_value_type
extract_meta_value<pbool_value_type>(const meta_function_const_argument_type& v) {
	if (v)
		return extract_pbool(*v);
	else {
		cerr << "Error extracting pbool from NULL argument." << endl;
		THROW_EXIT;
	}
}

template <>
preal_value_type
extract_meta_value<preal_value_type>(const meta_function_const_argument_type& v) {
	if (v)
		return extract_preal(*v);
	else {
		cerr << "Error extracting preal from NULL argument." << endl;
		THROW_EXIT;
	}
}

template <>
pstring_value_type
extract_meta_value<pstring_value_type>(
		const meta_function_const_argument_type& v) {
	if (v)
		return extract_pstring(*v);
	else {
		cerr << "Error extracting pstring from NULL argument." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
meta_function_return_type
make_meta_value(const pint_value_type v) {
	STACKTRACE_VERBOSE;
	return meta_function_return_type(new pint_const(v));
}

meta_function_return_type
make_meta_value(const pbool_value_type v) {
	STACKTRACE_VERBOSE;
	return meta_function_return_type(new pbool_const(v));
}

meta_function_return_type
make_meta_value(const preal_value_type v) {
	STACKTRACE_VERBOSE;
	return meta_function_return_type(new preal_const(v));
}

meta_function_return_type
make_meta_value(const pstring_value_type& v) {
	STACKTRACE_VERBOSE;
	return meta_function_return_type(new pstring_const(v));
}

//=============================================================================
void
verify_wrapped_args(const size_t a, const size_t f) {
	if (a != f) {
		cerr << "Error: got " << a << " function arguments, "
			"but expecting " << f << "." << endl;
		THROW_EXIT;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

