/**
	\file "common/ltdl-wrap.cc"
	$Id: ltdl-wrap.cc,v 1.3 2007/08/19 04:00:37 fang Exp $
 */

#include "common/ltdl-wrap.hh"
// #include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.tcc"
#include <iostream>
#include <map>
#include <list>
#include "util/stacktrace.hh"

/**
	Leave as 0 to use a consistent, but less informative error message.
	For debugging, define to 1.  
 */
#define	USE_LT_DLERROR			0

namespace HAC {
// using util::memory::never_ptr;
using util::memory::count_ptr;
using std::list;
using std::string;
#include "util/using_ostream.hh"

//=============================================================================
// static global structures

//=============================================================================
// class ltdl_token method definitions

ltdl_token::ltdl_token() { lt_dlinit(); }
ltdl_token::~ltdl_token() { lt_dlexit(); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Closes a dlopened module.  
	Make sure this is NOT called on a dlresident (permanent) module.
 */
void
ltdl_module_policy::operator () (const lt_dlhandle h) {
	if (h)
		lt_dlclose(h);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// typedef	util::remove_pointer<lt_dlhandle>::type		lt_dlref;
// wants to be const?
typedef	count_ptr<lt_dlhandle_struct, ltdl_module_policy>
							lt_dl_counted_handle;
typedef	std::map<std::string, lt_dl_counted_handle>	ltdl_handle_map_type;
typedef	std::list<lt_dl_counted_handle>			ltdl_module_list_type;

/**
	Named map to all modules.  
 */
static
ltdl_handle_map_type
opened_modules;

/**
	List of all opened modules.  
	This is also the list used in the global module search.  
 */
static
ltdl_module_list_type
module_list;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: pointer returned is NOT managed, perhaps it should be?
	Successfully opened module is added to front of search list.
	\param mname module name should exclude file extension.
 */
lt_dlhandle
ltdl_open_prepend(const string& mname) {
	// will be null-initialized if not found
	lt_dl_counted_handle& mh(opened_modules[mname]);
	if (!mh) {
		mh = lt_dl_counted_handle(lt_dlopenext(mname.c_str()));
		if (!mh) {
			// The precise error message varies across platforms,
			// depending on the dlopen library functions, 
			// so for sake of test cases, we craft our own message.
#if USE_LT_DLERROR
			cerr << lt_dlerror() << endl;
#else
			cerr << mname <<
				".{la,so,dylib} not found in library paths."
				<< endl;
#endif
			cerr << "Failed to dlopen `" << mname << "\'." << endl;
			return NULL;
		}
		module_list.push_front(mh);	// prepend
	}
	// INVARIANT(mh)
	return &*mh;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: pointer returned is NOT managed, perhaps it should be?
	Successfully opened module is added to end of search list.
	\param mname module name should exclude file extension.
	\return handle to successfully opened module or NULL.
 */
lt_dlhandle
ltdl_open_append(const string& mname) {
	// will be null-initialized if not found
	lt_dl_counted_handle& mh(opened_modules[mname]);
	if (!mh) {
		mh = lt_dl_counted_handle(lt_dlopenext(mname.c_str()));
		if (!mh) {
			// The precise error message varies across platforms,
			// depending on the dlopen library functions, 
			// so for sake of test cases, we craft our own message.
#if USE_LT_DLERROR
			// don't permanently remove, this can come in handy for
			// diagnosing unexpected prblems on various platforms.
			cerr << lt_dlerror() << endl;
#else
			cerr << mname <<
				".{la,so,dylib} not found in library paths."
				<< endl;
#endif
			cerr << "Failed to dlopen `" << mname << "\'." << endl;
			return NULL;
		}
		module_list.push_back(mh);	// append
	}
	// INVARIANT(mh)
	return &*mh;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Search through modules for symbols.  
	\param fname the name of the symbol, a function.
	Remember to pay attention to extern "C" linkage, and name-mangling.
 */
lt_dlsym_union
ltdl_find_sym(const string& fname) {
	typedef	ltdl_module_list_type::const_iterator	const_iterator;
	const char* const fs = fname.c_str();
	const_iterator i(module_list.begin()), e(module_list.end());
	lt_dlsym_union ret;
	ret.func_ptr = NULL;
	for ( ; i!=e; ++i) {
		const lt_dl_counted_handle& m(*i);
		NEVER_NULL(m);
		ret.nonfunc_ptr = lt_dlsym(&*m, fs);
		if (ret.nonfunc_ptr)
			return ret;
	}
	return ret;
}

//=============================================================================

}	// end namespace HAC

