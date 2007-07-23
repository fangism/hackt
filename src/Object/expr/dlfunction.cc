/**
	\file "Object/expr/dlfunction.cc"
	$Id: dlfunction.cc,v 1.1.2.1 2007/07/23 22:17:45 fang Exp $
 */

#include <iostream>
#include <map>
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
	Preloads map with function.  
	Calling this isn't necessary, only requirement is that 
	modules are loaded before searching for symbols.  
	\return 0 for success, anything else for error.
 */
int
register_chpsim_function(const string& fn, const chp_dlfunction_ptr_type fp) {
	STACKTRACE_VERBOSE;
	NEVER_NULL(fp);
	chp_mapped_func_ptr_type& mf(chp_function_map[fn]);
	if (!mf) {
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
 */
chp_dlfunction_ptr_type
lookup_chpsim_function(const std::string& fn) {
	typedef	chp_func_map_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE;
	chp_mapped_func_ptr_type& mf(chp_function_map[fn]);
	if (!mf) {
		lt_dlsym_union sym(ltdl_find_sym(fn));
		if (sym.func_ptr) {
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

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

