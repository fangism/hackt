/**
	\file "Object/expr/dlfunction.cc"
	$Id: dlfunction.cc,v 1.5 2010/09/21 00:18:13 fang Exp $
 */

#define	ENABLE_STACKTRACE	0

#include <iostream>
#include <map>
#include <typeinfo>		// for std::bad_cast
#include "Object/expr/dlfunction.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pstring_const.hh"
// #include "Object/expr/const_collection.hh"
#include "common/ltdl-wrap.hh"	// not actually needed
#include "util/macros.h"
#include "util/memory/count_ptr.hh"
#include "util/memory/excl_ptr.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
using util::memory::never_ptr;
using std::string;
#include "util/using_ostream.hh"

// typedef	never_ptr<chp_dlfunction_type>
typedef	chp_dlfunction_ptr_type
					chp_mapped_func_ptr_type;

typedef	std::map<string, chp_mapped_func_ptr_type>	
					chp_func_map_type;

static
chp_func_map_type			chp_function_map;

/**
	By default, echo each function name as it is registered.
 */
bool
ack_loaded_functions = true;


//=============================================================================
chp_function_registrar::chp_function_registrar(
		const string& fn, const chp_dlfunction_ptr_type fp) {
	if (register_chpsim_function(fn, fp)) {
		THROW_EXIT;
	}
}

chp_function_registrar::chp_function_registrar(
		const string& fn, chp_dlfunction_type* const fp) {
	if (register_chpsim_function(fn, chp_dlfunction_ptr_type(fp))) {
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
	if (ack_loaded_functions) {
		// confirmation:
		cout << "loaded function: `" << fn << "'." << endl;
	}
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
	CORRECTION: do NOT dl_find_sym, only use function registry interface.
 */
chp_dlfunction_ptr_type
lookup_chpsim_function(const std::string& fn) {
	typedef	chp_func_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE;
	const const_iterator i(chp_function_map.find(fn));
	if (i == chp_function_map.end()) {
		cerr << "ERROR: symbol `" << fn << "\' not found "
			"in presently loaded modules." << endl;
		return chp_dlfunction_ptr_type(NULL);
	} else {
		return i->second;
	}
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

