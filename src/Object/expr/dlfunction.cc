/**
	\file "Object/expr/dlfunction.cc"
	$Id: dlfunction.cc,v 1.1.2.5 2007/07/27 05:33:03 fang Exp $
 */

#include <iostream>
#include <map>
#include <typeinfo>		// for std::bad_cast
#include "Object/expr/dlfunction.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
// #include "Object/expr/const_collection.h"
#include "common/ltdl-wrap.h"
#include "util/macros.h"
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using util::memory::never_ptr;
using std::string;
#include "util/using_ostream.h"

typedef	never_ptr<const chp_dlfunction_type>
					chp_mapped_func_ptr_type;

typedef	std::map<string, chp_mapped_func_ptr_type>	
					chp_func_map_type;

static
chp_func_map_type			chp_function_map;

//=============================================================================
/**
	\throw a bad_cast on type-check failure.
 */
int_value_type
extract_int(const const_param& p) {
try {
	return IS_A(const pint_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting int, but got ") << endl;
	throw;		// re-throw
}
}

bool_value_type
extract_bool(const const_param& p) {
try {
	return IS_A(const pbool_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting bool, but got ") << endl;
	throw;		// re-throw
}
}

real_value_type
extract_real(const const_param& p) {
try {
	return IS_A(const preal_const&, p).static_constant_value();
} catch (std::bad_cast& e) {
	p.what(cerr << "Run-time error: expecting real, but got ") << endl;
	throw;		// re-throw
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
int_value_type
extract_chp_value<int_value_type>(const chp_function_const_argument_type& v) {
	if (v)
		return extract_int(*v);
	else {
		cerr << "Error extracting int from NULL argument." << endl;
		THROW_EXIT;
	}
}

template <>
bool_value_type
extract_chp_value<bool_value_type>(const chp_function_const_argument_type& v) {
	if (v)
		return extract_bool(*v);
	else {
		cerr << "Error extracting bool from NULL argument." << endl;
		THROW_EXIT;
	}
}

template <>
real_value_type
extract_chp_value<real_value_type>(const chp_function_const_argument_type& v) {
	if (v)
		return extract_real(*v);
	else {
		cerr << "Error extracting real from NULL argument." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
chp_function_return_type
make_chp_value(const int_value_type v) {
	return chp_function_return_type(new pint_const(v));
}

chp_function_return_type
make_chp_value(const bool_value_type v) {
	return chp_function_return_type(new pbool_const(v));
}

chp_function_return_type
make_chp_value(const real_value_type v) {
	return chp_function_return_type(new preal_const(v));
}

//=============================================================================
chp_function_registrar::chp_function_registrar(
		const string& fn, const chp_dlfunction_ptr_type fp) {
	if (register_chpsim_function(fn, fp)) {
		THROW_EXIT;
	}
}

chp_function_registrar::~chp_function_registrar() {
// un-register?
}

//=============================================================================
/**
	Preloads map with function.  
	Calling this isn't necessary, only requirement is that 
	modules are loaded before searching for symbols.  
	With this interface, functions can be mapped using names different
	than their symbol name, which circumvents symbol name mangling.  
	\return 0 for success, anything else for error.
 */
int
register_chpsim_function(const string& fn, const chp_dlfunction_ptr_type fp) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(fp);
	chp_mapped_func_ptr_type& mf(chp_function_map[fn]);
	if (!mf) {
#if 1
		// confirmation:
		cout << "loaded function: `" << fn << "'." << endl;
#endif
		mf = chp_mapped_func_ptr_type(fp);
	} else {
		cerr << "ERROR: CHP function symbol `" << fn <<
			"\' is already bound." << endl;
		return 1;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Automatically looks up function and caches resolved symbol.  
	The dlsym lookup is a last resort if it wasn't found in the
	managed function map already.  
 */
chp_dlfunction_ptr_type
lookup_chpsim_function(const std::string& fn) {
	typedef	chp_func_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE;
	chp_mapped_func_ptr_type& mf(chp_function_map[fn]);
	if (!mf) {
		lt_dlsym_union sym(ltdl_find_sym(fn));
		if (!sym.func_ptr) {
			cerr << "ERROR: symbol `" << fn << "\' not found "
				"in presently loaded modules." << endl;
			return NULL;
		}
		// here is the one type assumption: 
		// that registered functions have the correct prototype
		mf = chp_mapped_func_ptr_type(
			reinterpret_cast<chp_dlfunction_ptr_type>(sym.func_ptr));
	}
	return &*mf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print a list of all bound functions.  
 */
void
list_chpsim_functions(std::ostream& o) {
	typedef	chp_func_map_type::const_iterator	const_iterator;
	const_iterator i(chp_function_map.begin()), e(chp_function_map.end());
	o << "Functions bound to module symbols:" << endl;
	for ( ; i!=e; ++i) {
		NEVER_NULL(i->second);
		o << "\t" << i->first << endl;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

