/**
	\file "guile/hackt-documentation.cc"
	$Id: hackt-documentation.cc,v 1.3 2011/03/01 01:57:44 fang Exp $
 */

#include "guile/hackt-documentation.h"
#include "util/guile_STL.h"
#include "util/string.h"
#include "util/for_all.h"
#include "util/caller.h"

namespace HAC {
namespace guile_wrap {
using util::guile::make_scm;

//=============================================================================
/**
	Global documentation table.  
	0-initialized, set by first invocation of add_function_documentation().
 */
static
SCM
doc_hash_table;

/**
	Global HACKT scheme function registry.  
	Also initialized by add_function_documentation().
 */
static
SCM
doc_symbol;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
std::vector<scm_init_func_type>		local_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers documentation string from SCM_DEFINE with a global
	procedure-property table, and also with a hackt-only global table.  
 */
void
add_function_documentation(SCM func, const char* fname,
		const char* arglist, const char* docstring) {
if (strlen(docstring)) {
	if (!doc_hash_table) {
		// first entry
		doc_hash_table = scm_permanent_object(
			scm_c_make_hash_table(97));	// pick a prime, any
		doc_symbol = scm_permanent_object(
			scm_from_locale_symbol("documentation"));
	}
	std::string s(" - HACKT procedure: ");
	s = s + fname + " " + arglist + "\n" + docstring;
	scm_set_procedure_property_x(func, doc_symbol, make_scm(s));
	const SCM entry = scm_cons(make_scm(arglist), make_scm(docstring));
	scm_hashq_set_x(doc_hash_table, scm_from_locale_symbol(fname), entry);
}
// else nothing to document
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HAC_GUILE_DEFINE_PUBLIC(get_all_function_documentation, 
	"get-all-function-documentation", 0, 0, 0, (), local_registry,
"Return a global hash table with all HACKT Scheme extension functions."
"This is also used to generate documentation files for functions.")
{
	return doc_hash_table;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Call this to load the documentation functions.  
 */
void
init_documentation(void) {
	util::for_all(local_registry, util::caller());
}

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

